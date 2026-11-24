# Cómo hacer el plano en KiCad 7 — S.E.P.D.U.T.R. v3

Este documento acompaña a:

- `SEPDUTR_v3_ESP32CAM.ino` → el código de la ESP32-CAM.
- `SEPDUTR_v3_ESP32CAM.kicad_sch` → el plano ya hecho (28 componentes).
- `HUELLAS_Y_CONEXIONES.md` → tabla rápida de huellas y conexiones.
- `ALCANCE_Y_EFECTIVIDAD.md` → qué cambió, qué va a pasar y a qué distancia llega.

> **Importante:** el plano **ya está hecho**. Primero probá abrirlo (Parte A).
> La Parte B es el paso a paso para dibujarlo a mano, por si el archivo no te
> abre o si la profesora pide que lo hagas vos.

---

## Parte A — Abrir el plano que ya está hecho

1. Instalá **KiCad 7.0** (o superior) desde `https://www.kicad.org/download/`.
   Es gratis.
2. Guardá `SEPDUTR_v3_ESP32CAM.kicad_sch` en una carpeta propia.
3. Hacé doble clic sobre el archivo. Si Windows pregunta con qué programa
   abrirlo, elegí **KiCad Schematic Editor**.
4. Si KiCad te dice que el archivo es de una versión más nueva, actualizá KiCad.
5. Para verlo completo: `Ver → Ajustar a la hoja` (View → Zoom to Fit).
6. Para exportar: `Archivo → Trazar` (File → Plot), elegí **PDF** y guardá.

**No lo abras con Word, Bloc de notas ni Excel.** Es un archivo de texto, pero
KiCad lo interpreta; si lo editás a mano se rompe.

### Qué deberías ver al abrirlo

- Arriba a la izquierda: la **batería**, el **TP4056** y el **MT3608**.
- Centro: la **ESP32-CAM** (U1).
- Arriba al centro: el **HC-SR04** (J1).
- Derecha: el **PAM8403** (U2) y el **parlante** (SPK1).
- Abajo: el **DFPlayer Mini** (U5), el **PIR HC-SR501** (U6) y sus pasivos.
- Un bloque de notas al pie con las advertencias.

Si ves todo eso, el archivo está bien.

---

## Parte B — Hacerlo desde cero a mano

### B.1. Antes de empezar: qué es una “etiqueta de red”

Un plano eléctrico no siempre une los componentes con cables largos. Se usa una
**etiqueta de red** (net label): es un nombre que se le pone a un cable.

> Si dos cables tienen la misma etiqueta, KiCad entiende que están conectados,
> aunque estén en extremos opuestos de la hoja.

Ejemplo: al pin `GPIO12` de la ESP32-CAM le ponés la etiqueta `TRIG`, y al pin
`TRIG` del HC-SR04 le ponés la misma etiqueta `TRIG`. Para KiCad, esos dos pines
están unidos.

Esto hace el plano mucho más ordenado y fácil de leer.

### B.2. Crear el proyecto

1. Abrí KiCad.
2. `Archivo → Nuevo proyecto` (File → New Project).
3. Nombre: `SEPDUTR_v3_ESP32CAM`. Elegí una carpeta. Guardar.
4. En la ventana del proyecto, doble clic sobre `SEPDUTR_v3_ESP32CAM.kicad_sch`.
   Se abre el editor de esquemas.

### B.3. Configurar la hoja

1. `Archivo → Configuración de página` (File → Page Settings).
2. Tamaño de papel: **A3**. Aceptar.
3. En la pestaña del título podés escribir:
   - Título: `S.E.P.D.U.T.R. v3 - ESP32-CAM + IA + DFPlayer + PIR`
   - Empresa: `Taller 5 B`
   - Revisión: `F`
   - Fecha: la del día.

> **A3** es necesario: con A4 los 28 componentes no entran cómodos.

### B.4. Colocar los componentes

Para poner un componente: apretá **`A`** (o `Colocar → Agregar símbolo`), escribí
el nombre en el buscador, elegilo y hacé clic en la hoja. `Esc` para terminar.

Vas a necesitar esto:

| Qué | Símbolo de KiCad | Cantidad | Valor |
|---|---|---|---|
| Resistencia | `Device:R` | **8** | R1 1k, R2 2k, R4 220, R5 10k, R6 10k, R7 10k, **R8 1k, R9 1k** |
| Capacitor común | `Device:C` | 2 | C2 100nF, C5 2.2nF |
| Capacitor electrolítico | `Device:C_Polarized` | **6** | C1 470uF, C4 100uF, C6 10uF, C7 10uF, **C8 10uF, C9 470uF** |
| LED | `Device:LED` | 1 | LED2 rojo |
| Parlante | `Device:Speaker` | 1 | 8 Ω 3 W |
| Pulsador | `Switch:SW_Push` | 1 | — |
| Interruptor ON/OFF | `Switch:SW_SPST` | 1 | — |
| Batería | `Device:Battery_Cell` | 1 | 18650 3.7 V |
| Conector 2 pines | `Connector_Generic:Conn_01x02` | 2 | Parlante, interruptor |
| Conector 3 pines | `Connector_Generic:Conn_01x03` | 1 | **PIR HC-SR501** |
| Conector 4 pines | `Connector_Generic:Conn_01x04` | 3 | HC-SR04, MT3608, **DFPlayer** |
| Conector 6 pines | `Connector_Generic:Conn_01x06` | 1 | TP4056 |
| Conector 8 pines | `Connector_Generic:Conn_01x08` | 2 | ESP32-CAM (2 filas), PAM8403 |

> Los módulos se dibujan como **conectores**. No hace falta dibujar la plaquita
> entera: alcanza con mostrar los pines que se usan y ponerles una etiqueta con
> el nombre real.
>
> **Ojo con el DFPlayer:** el módulo real tiene 16 pines (2 filas de 8), pero
> este proyecto **solo usa 4** (VCC, GND, RX, DAC_L). Por eso en el esquema se
> dibuja como un conector de 4 pines. Si querés que se vea más realista, usá
> `Conn_02x08_Odd_Even` y dejá los demás pines con X.

Para cambiar el valor: pasá el mouse por encima y apretá **`V`**. Para cambiar la
referencia (R1, C1...): doble clic → campo `Reference`.

Al final, `Herramientas → Anotar esquema` (Tools → Annotate Schematic) numera
todo automáticamente.

### B.5. La tabla de conexiones (esto es lo más importante)

Cada pin que uses lleva un cable corto y una **etiqueta** (`L`). Los nombres
deben escribirse **exactamente igual** en los dos extremos.

#### ESP32-CAM (U1)

Solo se usan 6 GPIO. El resto lo ocupa la cámara o la PSRAM.

| Pin físico | Etiqueta | Va a |
|---|---|---|
| 5V | `+5V` | riel de 5 V |
| GND | `GND` | masa |
| GPIO12 | `TRIG` | TRIG del HC-SR04 y R7 |
| GPIO13 | `AUDIO_PWM` | R5 (respaldo PWM) **y** R9 (UART al DFPlayer) |
| GPIO14 | **`PIR_OUT`** | OUT del PIR HC-SR501 |
| GPIO15 | `BTN` | pulsador |
| GPIO2 | `ECHO` | divisor R1/R2 |
| GPIO4 | `LED_R` | R4 |
| 3V3 | — | sin uso (marcar con X) |
| U0TXD | — | sin uso (marcar con X) |
| U0RXD | — | sin uso (marcar con X) |

> **Ojo:** el orden puede cambiar según el fabricante. Mirá siempre las
> etiquetas impresas en tu placa. Los únicos pines libres para este proyecto
> son **12, 13, 14, 15, 2 y 4**; el resto lo usa la cámara o la PSRAM.
>
> **No usar GPIO16 ni GPIO17**: los ocupa la PSRAM y la placa no arranca.

#### HC-SR04 (J1)

| Pin | Etiqueta |
|---|---|
| VCC | `+5V` |
| TRIG | `TRIG` |
| ECHO | `ECHO_5V` |
| GND | `GND` |

#### PIR HC-SR501 (U6) — **nuevo en la v3**

| Pin | Etiqueta |
|---|---|
| VCC | `+5V` |
| OUT | **`PIR_OUT`** |
| GND | `GND` |

> El orden de los 3 pines **cambia entre fabricantes**. Verificá la serigrafía
> del módulo. Si lo conectás al revés, se quema.

#### DFPlayer Mini (U5) — **nuevo en la v3**

| Pin | Etiqueta |
|---|---|
| VCC | `+5V` |
| GND | `GND` |
| RX | **`DF_RX`** |
| DAC_L | **`DF_DAC`** |
| TX | sin uso (X) |
| SPK1 | sin uso (X) |
| SPK2 | sin uso (X) |
| BUSY | sin uso (X) |

> Si tu módulo **no tiene `DAC_L`**, usá `SPK1` y `SPK2` a través de un divisor
> `10 kΩ / 1 kΩ`, y de ahí al mismo C8. El resto del circuito no cambia.

#### PAM8403 (U2)

| Pin | Etiqueta |
|---|---|
| VCC | `+5V` |
| GND | `GND` |
| INL (o L) | `INL` |
| INR (o R) | `INR` |
| OUTL+ | `SPK_P` |
| OUTL− | `SPK_N` |
| OUTR+ | sin uso (X) |
| OUTR− | sin uso (X) |

#### Parlante (SPK1)

| Pin | Etiqueta |
|---|---|
| 1 | `SPK_P` |
| 2 | `SPK_N` |

#### Resistencias

| Resistencia | Pin 1 | Pin 2 |
|---|---|---|
| R1 1k | `ECHO_5V` | `ECHO` |
| R2 2k | `ECHO` | `GND` |
| R4 220 | `LED_R` | `LED_R_A` |
| R5 10k | `AUDIO_PWM` | `AUDIO_F` |
| R6 10k | `AUDIO_F` | `GND` |
| R7 10k | `TRIG` | `GND` |
| **R8 1k** | **`DF_AC`** | **`AUDIO_F`** |
| **R9 1k** | **`AUDIO_PWM`** | **`DF_RX`** |

#### Capacitores

| Capacitor | Pin + | Pin − | Nota |
|---|---|---|---|
| C1 470uF | `+5V` | `GND` | electrolítico, junto a la ESP32-CAM |
| C2 100nF | `+5V` | `GND` | cerámico, sin polaridad |
| C4 100uF | `+5V` | `GND` | electrolítico, junto al PAM8403 |
| C5 2.2nF | `AUDIO_F` | `GND` | cerámico, sin polaridad |
| C6 10uF | `AUDIO_F` | `INL` | **el + va del lado del PAM8403** |
| C7 10uF | `INR` | `GND` | **el + va del lado del PAM8403** |
| **C8 10uF** | **`DF_DAC`** | **`DF_AC`** | **el + va del lado del DFPlayer** |
| **C9 470uF** | **`+5V`** | **`GND`** | **junto al DFPlayer** |

> En C6 y C7 el positivo mira hacia el PAM8403. En C8 el positivo mira hacia el
> DFPlayer. Si los ponés al revés, se hinchan y se arruinan.

#### LED, pulsador, interruptor y batería

| Componente | Pin 1 | Pin 2 |
|---|---|---|
| LED2 rojo | `LED_R_A` (ánodo +) | `GND` (cátodo −) |
| SW2 pulsador | `BTN` | `GND` |
| SW1 ON/OFF | `V_PROT` | `V_SW` |
| BT1 18650 | `V_BAT` (+) | `BAT_NEG` (−) |

#### Alimentación

| Módulo | Pin | Etiqueta |
|---|---|---|
| TP4056 (U3) | BAT+ | `V_BAT` |
| TP4056 (U3) | BAT− | `BAT_NEG` |
| TP4056 (U3) | OUT+ | `V_PROT` |
| TP4056 (U3) | OUT− | `GND` |
| TP4056 (U3) | IN+ / IN− | sin uso (X) |
| MT3608 (U4) | IN+ | `V_SW` |
| MT3608 (U4) | IN− | `GND` |
| MT3608 (U4) | OUT+ | `+5V` |
| MT3608 (U4) | OUT− | `GND` |

> **Muy importante:** en el TP4056 **con protección**, `BAT−` y `OUT−` **no**
> son el mismo punto. `BAT−` va solamente a la batería; `OUT−` es la masa del
> circuito. Si los unís, anulás la protección de la batería.

### B.6. Dibujar cables y etiquetas

Para cada pin de la tabla:

1. Apretá **`W`** (cable). Hacé clic en la punta del pin, mové el mouse 5 mm
   hacia afuera y hacé otro clic. `Esc`.
2. Apretá **`L`** (etiqueta). Hacé clic en la punta libre del cable.
3. Escribí el nombre exacto (por ejemplo `TRIG`) y Enter.
4. Repetí con el otro componente que lleva la misma etiqueta.

Consejos:

- La etiqueta tiene que **tocar** el cable (el circulito del extremo). Si queda
  flotando, no conecta.
- Si dos cables se cruzan y **no** querés que se unan, no pongas un punto.
  KiCad solo une si hay un punto de unión (junction).
- Para mover un componente sin romper los cables: **`G`** (arrastrar).
- Para rotar: **`R`**. Para espejar: **`X`** o **`Y`**.
- Para borrar: seleccioná y **`Supr`**.

### B.7. Marcar los pines que no se usan

Apretá **`Q`** (sin conexión) y hacé clic en cada pin que no va a ningún lado:

- ESP32-CAM: `3V3`, `U0TXD`, `U0RXD`.
- PAM8403: `OUTR+` y `OUTR−`.
- TP4056: `IN+` e `IN−`.
- DFPlayer: `TX`, `SPK1`, `SPK2`, `BUSY`.

Aparece una cruz chica. Eso le dice a KiCad “este pin queda así a propósito”.

### B.8. Comprobar el plano (ERC)

1. `Herramientas → Anotar esquema` (para numerar R1, C1...).
2. `Inspeccionar → Comprobación de reglas eléctricas` (ERC).
3. Apretá **Run ERC**.

Es normal que aparezcan advertencias. Los **errores** que hay que corregir son:

- *Pin not connected* → te falta un cable o una etiqueta.
- *Label not connected* → la etiqueta no toca el cable.
- *Input power pin not driven* → agregá un símbolo `power:PWR_FLAG` en `+5V` y
  en `GND`. Es una bandera que le dice a KiCad “esta tensión viene de afuera”.

### B.9. Guardar y exportar

1. `Ctrl + S`.
2. `Archivo → Trazar` (File → Plot).
3. Formato: **PDF**. Marcá “Plot the whole sheet”. Guardar.
4. Para una imagen: `Archivo → Exportar → Imagen` (PNG).

---

## Parte C — Lista de materiales (BOM) para el plano

| Cant. | Componente | Detalle |
|---|---|---|
| 1 | ESP32-CAM | AI-Thinker, con cámara OV2640 |
| 1 | HC-SR04 | sensor de distancia ultrasónico |
| **1** | **HC-SR501** | **PIR, 5–7 m, 110°** |
| **1** | **DFPlayer Mini** | **reproductor MP3** |
| **1** | **microSD 1–8 GB** | **FAT32** |
| 1 | PAM8403 | módulo amplificador 5 V, 3 W + 3 W |
| 1 | Parlante | 8 Ω, 3 W, rango amplio |
| 1 | TP4056 | módulo de carga **con protección** |
| 1 | MT3608 | elevador ajustable, ajustar a 5.0 V |
| 1 | Batería 18650 | 3.7 V + portapila |
| 1 | Interruptor | ON/OFF |
| 1 | Pulsador | NA (normalmente abierto) |
| 1 | R1 | 1 kΩ |
| 1 | R2 | 2 kΩ |
| 1 | R4 | 220 Ω |
| 2 | R5, R6 | 10 kΩ |
| 1 | R7 | 10 kΩ |
| **2** | **R8, R9** | **1 kΩ** |
| 1 | C1 | 470 µF / 10 V (electrolítico) |
| 1 | C2 | 100 nF (cerámico) |
| 1 | C4 | 100 µF / 10 V (electrolítico) |
| 1 | C5 | 2.2 nF (cerámico) |
| 2 | C6, C7 | 10 µF / 16 V (electrolítico) |
| **1** | **C8** | **10 µF / 16 V (electrolítico)** |
| **1** | **C9** | **470 µF / 10 V (electrolítico)** |
| 1 | LED rojo | 5 mm |
| — | Cables, headers, protoboard o PCB | — |

> **Ya no se usan R3 ni el LED verde.**

---

## Parte D — Conexión física (para armar sin mirar el plano)

### D.1. Primero: ajustar el MT3608

1. Conectá la batería al TP4056 y la salida del TP4056 al MT3608 (con el
   interruptor en el medio).
2. **Sin conectar todavía la ESP32-CAM**, medí con un multímetro en `OUT+` y
   `OUT−` del MT3608.
3. Girá el tornillo del MT3608 hasta leer **5.0 V**.
4. Recién ahí conectá la ESP32-CAM, el HC-SR04, el PAM8403, el DFPlayer y el PIR.

> Si el MT3608 viene de fábrica con ~12 V y conectás la ESP32-CAM, la quemás.

### D.2. Alimentación

```
Batería +   → TP4056 BAT+
Batería −   → TP4056 BAT−      (¡NO unir con OUT−!)
TP4056 OUT+ → interruptor → MT3608 IN+
TP4056 OUT− → GND
MT3608 IN−  → GND
MT3608 OUT+ → +5V
MT3608 OUT− → GND
```

### D.3. ESP32-CAM

```
5V     → +5V
GND    → GND
GPIO12 → TRIG del HC-SR04
GPIO2  → punto medio del divisor (R1 1k / R2 2k)
GPIO13 → R5 10k  y  R9 1k
GPIO14 → OUT del PIR
GPIO15 → pulsador → GND
GPIO4  → R4 220 → LED rojo → GND
```

> El GPIO4 también enciende el **flash blanco integrado** de la ESP32-CAM.
> Es normal: cuando dispara, se prende el LED rojo y el flash a la vez.

### D.4. HC-SR04 (¡el ECHO no va directo!)

```
HC-SR04 VCC  → +5V
HC-SR04 GND  → GND
HC-SR04 TRIG → GPIO12
HC-SR04 ECHO → R1 1k → nodo ECHO → GPIO2
                        nodo ECHO → R2 2k → GND
```

El ECHO entrega 5 V y el GPIO2 aguanta 3.3 V. El divisor 1k/2k baja la tensión
a ~3.3 V. **Sin el divisor podés dañar la ESP32-CAM.**

### D.5. PIR HC-SR501

```
PIR VCC → +5V
PIR OUT → GPIO14
PIR GND → GND
```

Ajustes del módulo:
- **Tx (tiempo):** al mínimo (~3 s). Si lo dejás en 5 minutos, el sistema queda
  bloqueado después del primer disparo.
- **Sx (sensibilidad):** a media. Si lo ponés al máximo, dispara con el sol o
  con una cortina que se mueve.
- **Puente de modo:** ponelo en **H** (repetición) o **L** (un disparo). Para
  este proyecto, **H** anda mejor.
- **Calentamiento:** necesita **30–60 s** después de encender. El código ya
  espera 45 s antes de mirarlo.

### D.6. DFPlayer Mini

```
DFPlayer VCC   → +5V      (con C9 470 µF entre VCC y GND, pegado al módulo)
DFPlayer GND   → GND
DFPlayer RX    → R9 1k → GPIO13
DFPlayer DAC_L → C8 10uF (+) → R8 1k → nodo AUDIO_F
DFPlayer TX    → sin conectar
DFPlayer SPK1  → sin conectar
DFPlayer SPK2  → sin conectar
```

> **Verificá el pinout impreso en tu módulo.** Los clones cambian el orden de
> los pines. Si no tiene `DAC_L`, usá `SPK1` a través de un divisor
> `10 kΩ / 1 kΩ` y el mismo C8.

### D.7. Audio (mezcla de las dos fuentes)

```
GPIO13 → R5 10k → nodo AUDIO_F
nodo AUDIO_F → R6 10k → GND
nodo AUDIO_F → C5 2.2nF → GND
DFPlayer DAC_L → C8 10uF (+) → R8 1k → nodo AUDIO_F
nodo AUDIO_F → C6 10uF (+ hacia el PAM8403) → PAM8403 INL
PAM8403 INR → C7 10uF (+ hacia el PAM8403) → GND
PAM8403 VCC → +5V
PAM8403 GND → GND
PAM8403 OUTL+ → parlante pin 1
PAM8403 OUTL− → parlante pin 2
```

> El parlante va **entre OUTL+ y OUTL−**. No lo conectes a GND.
>
> El nodo `AUDIO_F` es el punto donde se juntan las dos fuentes. **No suenan
> las dos a la vez**: el código usa el DFPlayer si `USAR_DFPLAYER = 1`, y el PWM
> si `USAR_DFPLAYER = 0`. El cableado es el mismo en los dos casos.

### D.8. Capacitores de desacople

- **C1 470 µF y C2 100 nF** entre `+5V` y `GND`, lo más cerca posible de la
  ESP32-CAM.
- **C4 100 µF** entre `+5V` y `GND`, cerca del PAM8403.
- **C9 470 µF** entre `+5V` y `GND`, pegado al DFPlayer.
  Sin este capacitor el DFPlayer se reinicia o hace ruido al empezar cada pista.

### D.9. Prueba antes de cargar el código

1. Con el interruptor apagado, revisá con el multímetro que no haya un
   cortocircuito entre `+5V` y `GND`.
2. Encendé y medí 5.0 V en el pin 5V de la ESP32-CAM.
3. Medí 3.3 V en el pin 3V3 de la ESP32-CAM.
4. Recién ahí conectá el USB-TTL para cargar el código.

---

## Parte E — Preparar la tarjeta microSD

1. Formateá la tarjeta en **FAT32** (no exFAT). Con 1–2 GB es suficiente; el
   DFPlayer funciona mejor con tarjetas chicas.
2. Copiá en la **raíz** (no dentro de carpetas) los archivos:

| Archivo | Contenido |
|---|---|
| `0001.mp3` | Llamado de alarma real del tordo (1) |
| `0002.mp3` | Llamado de alarma real del tordo (2) |
| `0003.mp3` | Llamado de alarma real del tordo (3) |
| `0004.mp3` | Llamado de alarma real del tordo (4) |
| `0005.mp3` | Barrido 2,2 → 7,8 kHz |
| `0006.mp3` | Barrido 7,8 → 2,2 kHz |
| `0007.mp3` | Chirps de alarma |
| `0008.mp3` | Buzz 3,2 / 6,5 kHz |
| `0009.mp3` | Distress (opcional) |

3. Los nombres deben ser **exactamente** `0001.mp3` … `0009.mp3`.
4. Expulsá la tarjeta con seguridad antes de sacarla.

**De dónde sacar los llamados reales:** en `xeno-canto.org` buscá
**Molothrus bonariensis** y filtrá por “alarm call”. Descargá, recortá 1–2 s y
exportá como MP3. Citá la fuente y respetá la licencia en el informe.

**Cómo generar los tonos:** en Audacity, `Generar → Tono` (o `Chirp`) a la
frecuencia indicada, 1,5 s, y exportá como MP3 128 kbps.

---

## Parte F — Cargar el código en la ESP32-CAM

1. Instalá **Arduino IDE 2.x**.
2. `Archivo → Preferencias → Gestor de tarjetas adicionales` y pegá:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. `Herramientas → Placa → Gestor de tarjetas`: buscá **esp32** e instalá.
4. Abrí `SEPDUTR_v3_ESP32CAM.ino`.
5. Configurá:
   - Placa: **AI Thinker ESP32-CAM**
   - Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)**
   - PSRAM: **Enabled**
   - Velocidad: 115200
6. Conectá un adaptador **USB-TTL** (la ESP32-CAM no tiene USB):
   - USB-TTL 5V → ESP32-CAM 5V
   - USB-TTL GND → ESP32-CAM GND
   - USB-TTL TX → ESP32-CAM U0RXD
   - USB-TTL RX → ESP32-CAM U0TXD
7. Para entrar en modo carga: uní **GPIO0 a GND**, apretá **Reset** y soltá.
8. Dale **Subir**. Cuando termine, quitá el puente de GPIO0 y apretá Reset.
9. Abrí el **Monitor Serie** a 115200 para ver los mensajes.

> **Edge Impulse es opcional.** Si no tenés el header `ei_tordo_inferencing.h`,
> el código compila igual y funciona con PIR + distancia (sin confirmar
> especie). Es la forma más rápida de probar el hardware.

### F.1. Opciones de configuración del código

Al principio de `SEPDUTR_v3_ESP32CAM.ino` hay 3 interruptores:

```cpp
#define USAR_DFPLAYER   1   // 0 = solo PWM (modo v2)
#define USAR_PIR        1   // 0 = LED verde en GPIO14 (modo v2)
#define USAR_DEEP_SLEEP 0   // 1 = ahorro extremo (experimental)
```

Si algo falla, podés volver al modo v2 **sin recablear**:
- `USAR_DFPLAYER 0` → GPIO13 vuelve a ser PWM.
- `USAR_PIR 0` → GPIO14 vuelve a ser el LED verde (con R3 y LED1 puestos).

---

## Parte G — Problemas comunes

| Problema | Causa probable | Solución |
|---|---|---|
| El `.kicad_sch` no abre | KiCad viejo o archivo abierto con otro programa | Instalá KiCad 7+, abrilo con KiCad |
| “No encuentro el símbolo R” | Bibliotecas no cargadas | `Preferencias → Bibliotecas de símbolos`, agregar las estándar |
| Las etiquetas no conectan | La etiqueta no toca el cable | Acercala hasta que aparezca el circulito |
| ERC: “power input not driven” | Falta saber de dónde viene la tensión | Poné `power:PWR_FLAG` en `+5V` y `GND` |
| La ESP32-CAM no arranca | MT3608 mal ajustado o GPIO12 alto | Medí 5.0 V; revisá R7 10k a GND |
| No carga el código | GPIO0 no está a GND | Hacé el puente GPIO0–GND y Reset |
| Se reinicia cuando suena | Falta corriente | C1/C9 más grandes y cables más cortos |
| El DFPlayer no suena | Sin 5 V, R9 mal, MP3 mal nombrados, tarjeta exFAT | Medí 5 V; revisá R9; renombrá `0001.mp3`; formateá FAT32 |
| El DFPlayer hace ruido o se reinicia | Falta C9 o fuente débil | 470 µF pegado al módulo; fuente de 2 A |
| El audio suena muy bajo | Parlante o filtro | Revisá C6, R8/R5/R6 y que el parlante sea de 8 Ω |
| El PIR dispara solo | Calor del MT3608/PAM8403, sol, o sensibilidad alta | Alejalo de los módulos, bajá Sx, esperá 45 s |
| El PIR nunca dispara | Tx en 5 min o mal orientado | Poné Tx al mínimo |
| Dispara con personas | Es normal: el PIR no distingue especie | La IA filtra; sin IA, bajá `UMBRAL_CM` a 150 |
| El LED rojo no prende | LED al revés | Pata larga (+) al resistor, corta (−) a GND |

---

## Parte H — Opcional: símbolos propios

Si querés que el plano se vea como el archivo que te di (con las plaquitas
dibujadas en lugar de conectores):

1. `Herramientas → Editor de símbolos`.
2. `Archivo → Nuevo símbolo`.
3. Nombre: `ESP32CAM` (o `DFPLAYER`, `PIR`). Biblioteca: creá una nueva llamada
   `SEPDUTR` en la carpeta del proyecto.
4. Elegí “Unit 1”. Dibujá un rectángulo con la herramienta **Rectángulo**.
5. Con la herramienta **Pin** (`P`), agregá cada pin:
   - Nombre: `5V`, `GND`, `GPIO12`, etc.
   - Número: `1`, `2`, `3`...
   - Tipo eléctrico: **Passive**.
   - Orientación: izquierda o derecha.
6. Guardá.
7. En el esquema, `A` → buscá `SEPDUTR:ESP32CAM` → colocalo.

Para el informe alcanza con el esquemático. El *footprint* (la huella física)
solo se usa si vas a fabricar una PCB; no es necesario para aprobar el plano.

---

## Resumen

- El plano ya está en `SEPDUTR_v3_ESP32CAM.kicad_sch` (28 componentes).
- Si lo hacés a mano: colocás los símbolos, ponés un cable corto en cada pin y
  le escribís la etiqueta de la tabla. Las etiquetas iguales conectan.
- **Nuevo en la v3:** el PIR en GPIO14, el DFPlayer en GPIO13 (por R9) y su
  salida `DAC_L` por C8 + R8 al nodo de audio.
- Las huellas aproximadas ya están asignadas. Si las cambiás, usá
  `Herramientas → Asignar huellas`.
- Antes de armar: ajustá el MT3608 a 5.0 V, revisá el divisor del ECHO, no unas
  `BAT−` con `OUT−` y no te olvides del C9 junto al DFPlayer.
