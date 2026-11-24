# Conexiones paso a paso — S.E.P.D.U.T.R. v3

Guía de cableado completa. Sirve igual si armás en protoboard, en placa o con
cables dupont.

> **Diagramas:** `DIAGRAMA_PROTOBOARD.png` (vista física: cada cable de pin a
> pin) y `DIAGRAMA_CONEXIONES.png` (vista por bloques, con los nombres de las
> redes). Las dos muestran lo mismo que esta guía.

> **Los nombres `TRIG`, `ECHO`, `AUDIO_F`, etc. son etiquetas**, no pines.
> En el plano unen cables a distancia; en la realidad son **cables físicos**.

---

## 0. Tres reglas antes de tocar nada

1. **Todas las masas van juntas.** GND de la ESP32-CAM, del HC-SR04, del PIR,
   del DFPlayer, del PAM8403 y del MT3608 son el **mismo punto**.
2. **Todo se alimenta con 5 V**, menos el PIR (acepta 5 V) y la lógica de la
   ESP32-CAM (3,3 V internos). **Nada se alimenta con 3,3 V.**
3. **El TP4056 con protección tiene dos negativos distintos:** `BAT−` (solo
   batería) y `OUT−` (masa del circuito). **Nunca los unas.**

---

## 1. Alimentación

```
Batería 18650 (+) ──► TP4056  BAT+
Batería 18650 (−) ──► TP4056  BAT−      ← ¡NO unir con OUT−!

TP4056  OUT+ ──► interruptor (pin 1)
interruptor (pin 2) ──► MT3608  IN+
TP4056  OUT− ──► GND
MT3608  IN−  ──► GND

MT3608  OUT+ ──► +5V      (ajustado a 5,0 V)
MT3608  OUT−  ──► GND
```

**Antes de conectar nada más:** medí con el multímetro en `OUT+` / `OUT−` del
MT3608 y girá el tornillo hasta leer **5,0 V**. Viene de fábrica en ~12 V y
quema la ESP32-CAM.

---

## 2. ESP32-CAM (U1) — pinout físico

Mirando la placa con la cámara hacia vos y la antena arriba:

| Fila izquierda (de arriba a abajo) | Fila derecha (de arriba a abajo) |
|---|---|
| **5V** | 3V3 |
| **GND** | **GND** |
| **GPIO12** | GPIO16 ✗ |
| **GPIO13** | GPIO0 ✗ |
| **GPIO14** | GPIO5 ✗ |
| **GPIO15** | GPIO18 ✗ |
| **GPIO2** | GPIO19 ✗ |
| **GPIO4** | GPIO21 ✗ |

✗ = ocupado por la cámara o la PSRAM. **No conectar nada ahí.**

| Pin ESP32-CAM | Va a |
|---|---|
| 5V | +5V |
| GND (cualquiera de los dos) | GND |
| GPIO12 | TRIG del HC-SR04 **y** R7 10 k a GND |
| GPIO2 | punto medio del divisor R1 1 k / R2 2 k |
| GPIO15 | pulsador → GND |
| GPIO4 | R4 220 Ω → LED rojo → GND |
| GPIO14 | OUT del PIR |
| GPIO13 | R9 1 k → RX del DFPlayer **y** R5 10 k → filtro de audio |
| 3V3 | sin conectar |

> **GPIO12 tiene que tener R7 de 10 k a GND.** Si queda flotante, la placa no
> arranca (es un pin de arranque).

---

## 3. HC-SR04 (J1)

| Pin del sensor | Va a |
|---|---|
| VCC | +5V |
| TRIG | GPIO12 |
| ECHO | R1 1 k → **nodo ECHO** → GPIO2 |
| GND | GND |

Y en el nodo ECHO:

```
HC-SR04 ECHO ──► R1 1k ──┬──► GPIO2
                          │
                        R2 2k
                          │
                         GND
```

> El ECHO entrega **5 V** y el GPIO2 aguanta **3,3 V**. Sin el divisor podés
> quemar el pin. Es el error más común.

---

## 4. PIR HC-SR501 (U6)

| Pin del PIR | Va a |
|---|---|
| VCC | +5V |
| OUT | GPIO14 |
| GND | GND |

> **Verificá el orden impreso en el módulo.** Varía entre fabricantes. Si lo
> conectás al revés, se quema.
>
> Ajustes: **Tx** (tiempo) al mínimo (~3 s), **Sx** (sensibilidad) a media,
> puente en **H**. Necesita **45 s** de calentamiento (el código ya los espera).

---

## 5. DFPlayer Mini (U5)

| Pin del DFPlayer | Va a |
|---|---|
| VCC | +5V **con C9 470 µF a GND, pegado al módulo** |
| GND | GND |
| RX | R9 1 k → GPIO13 |
| DAC_L | C8 10 µF (+) → R8 1 k → nodo AUDIO_F |
| TX | sin conectar |
| SPK1 / SPK2 | sin conectar |
| BUSY | sin conectar |

> **No uses `SPK1`/`SPK2`** en esta versión: la señal sale por `DAC_L`, que es
> de línea y no necesita amplificación previa.
>
> Si tu módulo **no tiene `DAC_L`**: usá `SPK1` → divisor 10 k / 1 k → C8 → R8.
> Baja mucho el nivel; subí el volumen del DFPlayer al máximo (30).

---

## 6. Audio (la parte que hay que entender)

```
DFPlayer DAC_L ──► C8 10µF ──► R8 1k ──┐
   (+ al DFPlayer)                     │
                                       ├──► AUDIO_F ──► C6 10µF ──► PAM8403 INL
GPIO13 ──► R5 10k ─────────────────────┤     (+ al PAM8403)
                                       │
                     R6 10k ──► GND    │
                     C5 2,2nF ──► GND  │
```

En tabla:

| Componente | Pin 1 | Pin 2 |
|---|---|---|
| R5 10 k | GPIO13 | AUDIO_F |
| R6 10 k | AUDIO_F | GND |
| C5 2,2 nF | AUDIO_F | GND |
| C6 10 µF | **+ en INL** (lado PAM8403) | − en AUDIO_F |
| R8 1 k | DF_AC (salida de C8) | AUDIO_F |
| C8 10 µF | **+ en DAC_L** | − en DF_AC |

> **Ojo con C6:** el positivo va del lado del **PAM8403**, porque el amplificador
> tiene su entrada polarizada a ~2,5 V y el nodo AUDIO_F está a ~1,65 V.
>
> En el dibujo del esquema el símbolo del capacitor **no muestra el +** (es el
> símbolo genérico). Si armás una PCB, el **pad cuadrado (pin 1)** de C6 tiene
> que quedar del lado del PAM8403.

### PAM8403 (U2)

| Pin | Va a |
|---|---|
| VCC | +5V **con C4 100 µF a GND** |
| GND | GND |
| INL | C6 (+) |
| INR | C7 (+) → y C7 (−) a GND |
| OUTL+ | parlante pin 1 |
| OUTL− | parlante pin 2 |
| OUTR+ / OUTR− | sin conectar (es mono) |

> **El parlante va entre OUTL+ y OUTL−.** No lo conectes a GND: el PAM8403 es
> clase D con salida diferencial y puenteada. Conectarlo a masa lo daña.

---

## 7. LED, pulsador e interruptor

| Componente | Conexión |
|---|---|
| LED rojo | GPIO4 → R4 220 Ω → ánodo (+) → cátodo (−) → GND |
| Pulsador | GPIO15 ↔ GND (el código usa `INPUT_PULLUP`) |
| Interruptor | entre TP4056 OUT+ y MT3608 IN+ |

> La pata **larga** del LED es el ánodo (+).

---

## 8. Capacitores de desacople (no son opcionales)

| Capacitor | Entre | Dónde va físicamente |
|---|---|---|
| C1 470 µF | +5V y GND | pegado a la ESP32-CAM |
| C2 100 nF | +5V y GND | pegado a la ESP32-CAM |
| C4 100 µF | +5V y GND | pegado al PAM8403 |
| C9 470 µF | +5V y GND | **pegado al DFPlayer** |

Sin C9 el DFPlayer se reinicia o hace ruido al empezar cada pista. Sin C1 la
ESP32-CAM se reinicia cuando suena el parlante.

---

## 9. Orden de armado (no lo hagas todo junto)

| Paso | Qué conectás | Qué medís antes de seguir |
|---|---|---|
| 1 | Batería → TP4056 | 3,7–4,2 V en `OUT+`/`OUT−` |
| 2 | TP4056 → interruptor → MT3608 | **5,0 V** en `OUT+`/`OUT−` del MT3608 |
| 3 | ESP32-CAM sola | 5,0 V en `5V`, 3,3 V en `3V3`, 3 destellos del flash |
| 4 | HC-SR04 + R1/R2 + R7 | en el nodo ECHO nunca más de 3,3 V |
| 5 | PIR | esperá 45 s; al pasar la mano, OUT pasa a 3,3 V |
| 6 | DFPlayer + C9 + R9 | con el pulsador tiene que sonar |
| 7 | PAM8403 + C4 + parlante | sonido fuerte y limpio |
| 8 | Todo junto | 5,0 V estable mientras suena |

---

## 10. Checklist final antes de encender

- [ ] No hay continuidad entre `+5V` y `GND` (sin el interruptor cerrado).
- [ ] `BAT−` del TP4056 **no** está unido a `OUT−`.
- [ ] El MT3608 está en 5,0 V.
- [ ] El divisor R1/R2 está entre el ECHO y el GPIO2.
- [ ] R7 10 k está entre GPIO12 y GND.
- [ ] El parlante está entre OUTL+ y OUTL− (no a GND).
- [ ] C9 está junto al DFPlayer.
- [ ] La microSD está en FAT32 con `0001.mp3`…`0009.mp3` en la raíz.
- [ ] El PIR tiene Tx al mínimo y 45 s de calentamiento.

---

## 11. Errores que queman componentes

| Error | Consecuencia |
|---|---|
| MT3608 sin ajustar (~12 V) | quema la ESP32-CAM |
| ECHO del HC-SR04 directo al GPIO2 | quema el pin |
| PIR conectado al revés | quema el PIR |
| DFPlayer al revés | quema el DFPlayer |
| `BAT−` unido a `OUT−` | anula la protección de la batería |
| Parlante a GND en vez de OUTL+/OUTL− | no suena y puede dañar el PAM8403 |
| Alimentar el DFPlayer con 3,3 V | no funciona |
| GPIO16 o GPIO17 usados | la placa no arranca (PSRAM) |
| GPIO12 sin R7 a GND | la placa no arranca |
| Sonar sin C1 ni C9 | reinicios aleatorios |
