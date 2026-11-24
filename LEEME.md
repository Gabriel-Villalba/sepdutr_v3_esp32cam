# S.E.P.D.U.T.R. v3 — LEEME

Sistema Electrónico Portátil de Disuasión para Tordos Renegridos.
**ESP32-CAM + IA de visión + DFPlayer (llamados reales) + PAM8403 + parlante.**

---

## Archivos de esta carpeta

| Archivo | Qué es |
|---|---|
| `SEPDUTR_v3_ESP32CAM.ino` | Código nuevo (v3) para la ESP32-CAM |
| `SEPDUTR_v3_ESP32CAM.kicad_sch` | Plano KiCad 7 (28 componentes, 28 huellas) |
| `ALCANCE_Y_EFECTIVIDAD.md` | Qué cambió, qué va a pasar y a qué distancia llega |
| `HUELLAS_Y_CONEXIONES.md` | Huellas y conexiones red por red |
| `COMO_HACER_EL_PLANO_EN_KICAD_7.md` | Paso a paso para dibujar el plano |
| `LEEME.md` | Este documento |

---

## Qué cambió respecto de la v2

1. **DFPlayer Mini** con tarjeta microSD: reproduce **llamados de alarma reales**
   del tordo, no solo tonos.
2. **Audio en ráfagas** con silencios aleatorios: rompe la habituación.
3. **PIR HC-SR501**: detecta a 5–7 m y 110° (la v2 llegaba a 1,5 m).
4. **Tres etapas de confirmación**: PIR → distancia → IA (3 fotos, 2 positivas).
5. **Modo intenso automático** si el ave insiste.
6. Se eliminó el **LED verde**: GPIO14 ahora es el PIR. El LED rojo (GPIO4)
   indica todo con parpadeos y enciende el flash blanco.

---

## Lista de materiales (BOM)

### Módulos

| Cant. | Componente | Detalle |
|---|---|---|
| 1 | ESP32-CAM | AI-Thinker, cámara OV2640 |
| 1 | HC-SR04 | sensor ultrasónico |
| 1 | **HC-SR501** | PIR, 5–7 m, 110°, salida 3,3 V |
| 1 | **DFPlayer Mini** | reproductor MP3 + tarjeta microSD |
| 1 | **microSD** | 1–8 GB, FAT32 (mejor 2 GB) |
| 1 | PAM8403 | amplificador 5 V, 3 W + 3 W |
| 1 | Parlante | 8 Ω, 3 W |
| 1 | TP4056 | cargador **con protección** |
| 1 | MT3608 | elevador ajustable a **5,0 V** |
| 1 | Batería 18650 | 3,7 V + portapila |
| 1 | Interruptor | ON/OFF |
| 1 | Pulsador | NA |

### Componentes pasivos

| Cant. | Componente | Valor |
|---|---|---|
| 1 | R1 | 1 kΩ |
| 1 | R2 | 2 kΩ |
| 1 | R4 | 220 Ω |
| 2 | R5, R6 | 10 kΩ |
| 1 | R7 | 10 kΩ (pull-down GPIO12) |
| 2 | **R8, R9** | 1 kΩ |
| 1 | C1 | 470 µF / 10 V |
| 1 | C2 | 100 nF |
| 1 | C4 | 100 µF / 10 V |
| 1 | C5 | 2,2 nF |
| 2 | C6, C7 | 10 µF / 16 V |
| 1 | **C8** | 10 µF / 16 V |
| 1 | **C9** | 470 µF / 10 V |
| 1 | LED rojo | 5 mm |

> **Ya no se usan R3 ni el LED verde.** Si los tenés, guardalos.

---

## Mapa de pines

| GPIO | Función | Nota |
|---|---|---|
| 12 | HC-SR04 TRIG | R7 10 kΩ a GND (pin de arranque) |
| 2 | HC-SR04 ECHO | **divisor R1 1 kΩ / R2 2 kΩ** |
| 15 | Pulsador | a GND, `INPUT_PULLUP` |
| 4 | LED rojo + flash | R4 220 Ω |
| 14 | **PIR OUT** | HC-SR501, salida 3,3 V |
| 13 | **DFPlayer RX** (UART2 TX) | por R9 1 kΩ |
| — | DFPlayer DAC_L | C8 10 µF → R8 1 kΩ → nodo de audio |

> **No usar GPIO16 / GPIO17**: los ocupa la PSRAM.
> **No usar GPIO0**: es el reloj de la cámara.
> La tarjeta SD del ESP32-CAM **no se usa**; sus pines son los de arriba.

---

## Conexionado físico

### 1. Alimentación

```
Batería +   → TP4056 BAT+
Batería −   → TP4056 BAT−        (¡NO unir con OUT−!)
TP4056 OUT+ → interruptor → MT3608 IN+
TP4056 OUT− → GND
MT3608 IN−  → GND
MT3608 OUT+ → +5V
MT3608 OUT− → GND
```

**Antes de conectar nada más:** medí con el multímetro en `OUT+` / `OUT−` del
MT3608 y ajustá el tornillo hasta leer **5,0 V**.

### 2. ESP32-CAM

```
5V     → +5V
GND    → GND
GPIO12 → TRIG del HC-SR04
GPIO2  → punto medio del divisor R1/R2
GPIO14 → OUT del PIR
GPIO15 → pulsador → GND
GPIO4  → R4 220 Ω → LED rojo → GND
GPIO13 → R9 1 kΩ → RX del DFPlayer
```

### 3. HC-SR04

```
VCC  → +5V
GND  → GND
TRIG → GPIO12
ECHO → R1 1 kΩ → nodo ECHO → GPIO2
       nodo ECHO → R2 2 kΩ → GND
```

### 4. PIR HC-SR501

```
VCC → +5V
OUT → GPIO14
GND → GND
```

> El HC-SR501 tiene dos potenciómetros: **sensibilidad** (Sx) y **tiempo**
> (Tx). Poné el tiempo al mínimo (~3 s) y la sensibilidad a media.
> Necesita **30–60 s** para estabilizarse después de encender.

### 5. DFPlayer Mini

```
VCC   → +5V        (con C9 470 µF entre VCC y GND, cerca del módulo)
GND   → GND
RX    → R9 1 kΩ → GPIO13
DAC_L → C8 10 µF (+) → R8 1 kΩ → nodo AUDIO_F
TX    → sin conectar
SPK1  → sin conectar
SPK2  → sin conectar
```

> **Verificá el pinout impreso en tu módulo.** Algunos clones tienen
> `DAC_L`/`DAC_R` en otro orden. Si tu módulo no tiene `DAC_L`, usá `SPK1`
> pasando por un divisor 10 kΩ / 1 kΩ y el mismo C8.

### 6. Audio

```
GPIO13 → R5 10 kΩ → nodo AUDIO_F
nodo AUDIO_F → R6 10 kΩ → GND
nodo AUDIO_F → C5 2,2 nF → GND
nodo AUDIO_F → C6 10 µF (+) → PAM8403 INL
PAM8403 INR → C7 10 µF (+) → GND
PAM8403 VCC → +5V
PAM8403 GND → GND
PAM8403 OUTL+ → parlante pin 1
PAM8403 OUTL− → parlante pin 2
```

> El parlante va **entre OUTL+ y OUTL−**. No lo conectes a GND.
> El nodo `AUDIO_F` mezcla las dos fuentes: el PWM (respaldo) y el DFPlayer.

### 7. Capacitores de desacople

- **C1 470 µF + C2 100 nF** entre `+5V` y `GND`, junto a la ESP32-CAM.
- **C4 100 µF** entre `+5V` y `GND`, junto al PAM8403.
- **C9 470 µF** entre `+5V` y `GND`, junto al DFPlayer.

---

## Tarjeta microSD del DFPlayer

Formateá la tarjeta en **FAT32** y copiá los archivos en la raíz:

| Archivo | Contenido | Duración |
|---|---|---|
| `0001.mp3` | Llamado de alarma real del tordo (1) | 1–2 s |
| `0002.mp3` | Llamado de alarma real del tordo (2) | 1–2 s |
| `0003.mp3` | Llamado de alarma real del tordo (3) | 1–2 s |
| `0004.mp3` | Llamado de alarma real del tordo (4) | 1–2 s |
| `0005.mp3` | Barrido 2,2 → 7,8 kHz | 1,5 s |
| `0006.mp3` | Barrido 7,8 → 2,2 kHz | 1,5 s |
| `0007.mp3` | Chirps de alarma | 1,5 s |
| `0008.mp3` | Buzz 3,2 / 6,5 kHz | 1,5 s |
| `0009.mp3` | Distress (opcional) | 2 s |

**Cómo conseguir los llamados reales:** buscá en xeno-canto.org el nombre
científico **Molothrus bonariensis** y descargá grabaciones de “alarm call”.
Respetá la licencia de cada grabación y citá la fuente en el informe.

**Cómo generar los tonos:** en Audacity, `Generar → Tono` (o `Chirp`) a la
frecuencia indicada, 1,5 s, y exportá como MP3.

> Los archivos deben llamarse exactamente `0001.mp3` … `0009.mp3`.
> Si el DFPlayer no los encuentra, revisá que estén en la raíz y que la
> tarjeta sea FAT32 (no exFAT).

---

## Cargar el código

1. Arduino IDE 2.x.
2. Preferencias → Gestor de tarjetas adicionales:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Herramientas → Placa → **AI Thinker ESP32-CAM**.
4. Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)**.
5. PSRAM: **Enabled**.
6. Adaptador USB-TTL (la placa no tiene USB):
   - 5V → 5V, GND → GND, TX → U0RXD, RX → U0TXD.
7. Para cargar: uní **GPIO0 a GND**, apretá Reset y soltá.
8. Subir. Al terminar, quitá el puente y apretá Reset.
9. Monitor Serie a **115200**.

> Si no tenés el header de Edge Impulse, el código compila igual y funciona
> con PIR + distancia (sin confirmar especie).

---

## Secuencia de prueba recomendada

1. **Solo alimentación:** medí 5,0 V en el pin 5V de la ESP32-CAM y 3,3 V en 3V3.
2. **Sin audio:** abrí el Monitor Serie. Tiene que decir `Camara: OK` y
   `Listo.`
3. **PIR:** pasá la mano frente al PIR. Debe imprimir `PIR: movimiento` y la
   distancia.
4. **DFPlayer:** apretá el pulsador. Debe sonar una ráfaga. Si no suena,
   revisá R9, C9 y los nombres de los MP3.
5. **IA:** poné una foto del tordo en un celular frente a la cámara y probá.
6. **Campo:** dejalo 3 días y compará con la línea base.

---

## Advertencias críticas

| Riesgo | Qué hacer |
|---|---|
| **MT3608 viene en ~12 V** | Ajustalo a **5,0 V antes** de conectar la ESP32-CAM |
| **ECHO del HC-SR04 es de 5 V** | Divisor **R1 1 kΩ / R2 2 kΩ** al GPIO2 |
| **TP4056 con protección** | `BAT−` va **solo a la batería**; `OUT−` es GND. **No los unas** |
| **GPIO12 es pin de arranque** | R7 10 kΩ a GND |
| **GPIO16/17** | No usarlos: los ocupa la PSRAM |
| **DFPlayer se reinicia o hace ruido** | C9 470 µF junto al módulo y cables cortos |
| **El audio corta la WiFi o reinicia** | C1 470 µF + cables gruesos; el pico del PAM8403 es alto |
| **El PIR dispara con personas** | Es normal: la IA filtra. Sin IA, bajá `UMBRAL_CM` a 150 |

---

## Problemas comunes

| Síntoma | Causa probable | Solución |
|---|---|---|
| No compila | Falta la librería de la cámara | Instalar el core ESP32 |
| `Camara: ERROR` | PSRAM deshabilitada o placa equivocada | PSRAM Enabled, AI Thinker |
| No suena nada | DFPlayer sin 5 V, R9 mal, MP3 mal nombrados | Medí 5 V, revisá R9 y `0001.mp3` |
| Suena entrecortado | C9 insuficiente o fuente débil | 470 µF + fuente de 2 A |
| Dispara con todo | Sin IA y PIR muy sensible | Bajá la sensibilidad del PIR |
| No dispara nunca | PIR en tiempo muy largo o mal orientado | Ajustá Tx al mínimo |
| Se reinicia al sonar | Caída de tensión | C1/C9 más grandes, cables cortos |
| El LED rojo queda prendido | El audio está bloqueado | Reiniciá; revisá la tarjeta SD |

---

## Resumen

- La v3 **no cambia el concepto**: sigue siendo disuadir con sonido audible.
- Cambia **cómo suena** (llamados reales, ráfagas), **cuándo suena** (PIR +
  distancia + IA) y **cuánto insiste** (modo intenso).
- El radio de disparo pasa de ~1,5 m a **~3 m**; el sonido molesta hasta
  **15–25 m** y sobresalta a **4–8 m**.
- Para más alcance: **bocina** (duplica) y, si hace falta, **PAM8610 + bocina**
  (4×).
