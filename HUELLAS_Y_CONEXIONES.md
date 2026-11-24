# Huellas y conexiones — S.E.P.D.U.T.R. v3 (KiCad 7)

Este documento explica, componente por componente:

1. Qué **huella** (footprint) le asigné en el archivo `.kicad_sch`.
2. Cómo **verificar o cambiar** esa huella dentro de KiCad 7.
3. Cómo se **conecta** cada pin (tabla de redes, red por red).
4. Cómo pasar del esquema a la **PCB**.

> El archivo `SEPDUTR_v3_ESP32CAM.kicad_sch` ya tiene las **28 huellas**
> asignadas (28 símbolos = 28 huellas). No hace falta que las cargues a mano,
> pero acá está el paso a paso por si querés cambiarlas o entender qué elegí.

---

## 1. Símbolo vs. huella (para no confundirse)

| Concepto | Qué es | Dónde se ve |
|---|---|---|
| **Símbolo** | El dibujo del componente en el esquema (rayitas, pines, nombre) | Editor de esquemas |
| **Huella** (footprint) | El dibujo físico de las pistas y agujeros en la placa | Editor de PCB (Pcbnew) |

El símbolo dice **qué hace** el componente; la huella dice **dónde va soldado**.
Una resistencia puede tener el mismo símbolo y distintas huellas según el tamaño
(1/4 W, 1/2 W, SMD, etc.).

---

## 2. Tabla de huellas asignadas (28 componentes)

Todas son huellas que vienen **incluidas en KiCad 7** (verificadas en las
bibliotecas oficiales 7.0.x).

### 2.1. Módulos

| Ref. | Componente | Huella asignada | Observación |
|---|---|---|---|
| U1 | ESP32-CAM | `Connector_PinHeader_2.54mm:PinHeader_2x08_P2.54mm_Vertical` | **Aproximada.** La placa real tiene 2 filas de 8 pines separadas ~22.9 mm, no 2.54 mm. Para una PCB real conviene usar 2 tiras `PinHeader_1x08` o crear una huella propia. |
| J1 | HC-SR04 | `Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical` | 4 pines a 2.54 mm. Coincide bien. |
| U2 | PAM8403 | `Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical` | Si tu módulo es 2x4, usá `PinHeader_2x04_P2.54mm_Vertical`. |
| **U5** | **DFPlayer Mini** | `Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical` | **Solo se usan 4 pines** (VCC, GND, RX, DAC_L). El módulo real tiene 2 filas de 8. La huella es referencial: en la práctica va con cables dupont. |
| **U6** | **HC-SR501 (PIR)** | `Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical` | 3 pines: VCC, OUT, GND. Coincide bien. |
| SPK1 | Parlante 8 Ω 3 W | `Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical` | Conector o bornera de 2 pines. |

### 2.2. Alimentación

| Ref. | Componente | Huella asignada | Observación |
|---|---|---|---|
| BT1 | Batería 18650 | `Battery:BatteryHolder_Keystone_1042_1x18650` | Portapila Keystone 1042. Si no la conseguís, usá un conector de 2 pines. |
| U3 | TP4056 con protección | `Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical` | El módulo tiene 6 pads (IN+, IN−, B+, B−, OUT+, OUT−). El símbolo usa B+/B−/OUT+/OUT− y deja IN+/IN− sin conexión (la carga se hace por el USB del módulo). |
| U4 | MT3608 | `Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical` | 4 pads: IN+, IN−, OUT+, OUT−. |
| SW1 | Interruptor ON/OFF | `Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical` | Conector para llevar el interruptor al gabinete. |

### 2.3. Resistencias (1/4 W)

| Ref. | Valor | Función | Huella asignada |
|---|---|---|---|
| R1 | 1 kΩ | divisor del ECHO | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| R2 | 2 kΩ | divisor del ECHO | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| R4 | 220 Ω | LED rojo | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| R5 | 10 kΩ | filtro PWM (respaldo) | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| R6 | 10 kΩ | filtro PWM (respaldo) | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| R7 | 10 kΩ | pull-down de GPIO12 | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| **R8** | **1 kΩ** | **atenuador de la salida DAC del DFPlayer** | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| **R9** | **1 kΩ** | **protección de la línea UART hacia el DFPlayer** | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |

> `DIN0207` = resistencia axial de 1/4 W. Distancia entre patas: 10.16 mm
> (0.4 pulgadas). Si tus resistencias son más chicas, usá
> `R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal`.
>
> **Ya no se usa R3** (era del LED verde, que se eliminó en la v3).

### 2.4. Capacitores

| Ref. | Valor | Huella asignada | Tipo |
|---|---|---|---|
| C1 | 470 µF | `Capacitor_THT:CP_Radial_D8.0mm_P3.50mm` | Electrolítico, 8 mm de diámetro, 3.5 mm entre patas |
| C2 | 100 nF | `Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm` | Cerámico, 5 mm entre patas |
| C4 | 100 µF | `Capacitor_THT:CP_Radial_D6.3mm_P2.50mm` | Electrolítico, 6.3 mm, 2.5 mm |
| C5 | 2.2 nF | `Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm` | Cerámico, 5 mm |
| C6 | 10 µF | `Capacitor_THT:CP_Radial_D6.3mm_P2.50mm` | Electrolítico, 6.3 mm, 2.5 mm |
| C7 | 10 µF | `Capacitor_THT:CP_Radial_D6.3mm_P2.50mm` | Electrolítico, 6.3 mm, 2.5 mm |
| **C8** | **10 µF** | `Capacitor_THT:CP_Radial_D6.3mm_P2.50mm` | Electrolítico: acopla la salida DAC del DFPlayer |
| **C9** | **470 µF** | `Capacitor_THT:CP_Radial_D8.0mm_P3.50mm` | Electrolítico: estabiliza la alimentación del DFPlayer |

> En los electrolíticos, la huella tiene un **+** y un **−**. El positivo va
> del lado que indica la tabla de conexiones. **C8: el + va del lado del
> DFPlayer (DAC_L).**

### 2.5. LED y pulsador

| Ref. | Componente | Huella asignada |
|---|---|---|
| LED2 | LED rojo 5 mm | `LED_THT:LED_D5.0mm` |
| SW2 | Pulsador 6 mm | `Button_Switch_THT:SW_PUSH_6mm` |

> En el LED, la pata larga es el **ánodo (+)** y la corta el **cátodo (−)**.
>
> **Ya no existe LED1** (el verde). El GPIO14 ahora es la entrada del PIR.

---

## 3. Cómo ver o cambiar las huellas en KiCad 7

### Opción A: ver la huella de un componente

1. Abrí `SEPDUTR_v3_ESP32CAM.kicad_sch`.
2. Hacé **doble clic** sobre el componente.
3. En la ventana de propiedades buscá el campo **Footprint**.
4. Ahí aparece el nombre completo, por ejemplo:
   `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal`.

### Opción B: asignar huellas a todos de una vez

1. `Herramientas → Asignar huellas` (Tools → Assign Footprints).
2. Se abre una ventana con tres columnas:
   - **Izquierda:** lista de símbolos (U1, R1, C1...).
   - **Centro:** bibliotecas y huellas.
   - **Derecha:** la huella elegida para ese símbolo.
3. Hacé clic en un símbolo de la izquierda.
4. En el centro, escribí en el filtro el nombre de la huella (por ejemplo
   `PinHeader_1x04`) y elegila.
5. Apretá **Aplicar** (o doble clic sobre la huella).
6. Repetí con los demás.
7. `Aplicar, Guardar y Continuar`.
8. `Ctrl + S`.

> Si no aparece ninguna huella, revisá que estén instaladas las bibliotecas
> estándar: `Preferencias → Administrar bibliotecas de huellas`.

### Opción C: cambiar solo una

1. Doble clic en el componente.
2. Campo **Footprint** → botón de biblioteca.
3. Buscá y elegí.
4. Aceptar.

---

## 4. Tabla de conexiones (red por red)

Esta es la lista exacta de qué pin se une con qué pin. En KiCad, dos pines con
la **misma etiqueta** quedan conectados aunque no haya un cable entre ellos.

### 4.1. Alimentación

| Red | Se conecta a |
|---|---|
| `+5V` | U1 pin 5V · J1 VCC · U2 VCC · C1 (+) · C2 pin 1 · C4 (+) · U4 OUT+ · **U5 VCC** · **C9 (+)** · **U6 VCC** |
| `GND` | U1 GND (×2) · J1 GND · U2 GND · C1 (−) · C2 pin 2 · C4 (−) · C5 pin 2 · C7 pin 2 · R2 pin 2 · R6 pin 2 · R7 pin 2 · LED2 (−) · SW2 pin 2 · U3 OUT− · U4 IN− · U4 OUT− · **U5 GND** · **C9 (−)** · **U6 GND** |
| `+3V3` | U1 3V3 (sin uso, marcar con X) |
| `V_BAT` | BT1 (+) · U3 BAT+ |
| `BAT_NEG` | BT1 (−) · U3 BAT− |
| `V_PROT` | U3 OUT+ · SW1 pin 1 |
| `V_SW` | SW1 pin 2 · U4 IN+ |

### 4.2. Sensores y control

| Red | Se conecta a |
|---|---|
| `TRIG` | U1 GPIO12 · J1 TRIG · R7 pin 1 |
| `ECHO_5V` | J1 ECHO · R1 pin 1 |
| `ECHO` | R1 pin 2 · R2 pin 1 · U1 GPIO2 |
| `BTN` | U1 GPIO15 · SW2 pin 1 |
| **`PIR_OUT`** | **U6 OUT · U1 GPIO14** |
| `LED_R` | U1 GPIO4 · R4 pin 1 |
| `LED_R_A` | R4 pin 2 · LED2 (+) |

### 4.3. Audio (dos fuentes mezcladas)

| Red | Se conecta a |
|---|---|
| `AUDIO_PWM` | U1 GPIO13 · R5 pin 1 · **R9 pin 1** |
| **`DF_RX`** | **R9 pin 2 · U5 RX** |
| **`DF_DAC`** | **U5 DAC_L · C8 (+)** |
| **`DF_AC`** | **C8 (−) · R8 pin 1** |
| `AUDIO_F` | R5 pin 2 · R6 pin 1 · C5 pin 1 · C6 pin 1 · **R8 pin 2** |
| `INL` | C6 pin 2 · U2 INL |
| `INR` | C7 pin 1 · U2 INR |
| `SPK_P` | U2 OUTL+ · SPK1 pin 1 |
| `SPK_N` | U2 OUTL− · SPK1 pin 2 |

> **Cómo funciona la mezcla:** el DFPlayer entrega audio analógico por `DAC_L`,
> pasa por C8 (quita la continua), R8 (baja el nivel) y llega al nodo
> `AUDIO_F`. El PWM de respaldo entra al mismo nodo por R5/R6/C5. Del nodo
> `AUDIO_F` sale C6 hacia `INL` del PAM8403. **Nunca se usan las dos fuentes
> al mismo tiempo**: el código elige una u otra.

### 4.4. Resumen de pines usados del ESP32-CAM

| GPIO | Red | Función |
|---|---|---|
| 12 | `TRIG` | disparo del HC-SR04 |
| 2 | `ECHO` | lectura del HC-SR04 (por divisor) |
| 15 | `BTN` | pulsador |
| 4 | `LED_R` | LED rojo + flash interno |
| 14 | `PIR_OUT` | salida del PIR |
| 13 | `AUDIO_PWM` | UART TX al DFPlayer / PWM de respaldo |

### 4.5. Pines marcados como “sin conexión” (7 en total)

| Componente | Pines con X |
|---|---|
| U1 ESP32-CAM | `3V3`, `U0TXD`, `U0RXD` |
| U2 PAM8403 | `OUTR+`, `OUTR−` (canal derecho, no se usa: es mono) |
| U3 TP4056 | `IN+`, `IN−` (la carga entra por el USB del módulo) |

### 4.6. Cómo se dibuja en KiCad (resumen)

1. Colocá los símbolos con **`A`**.
2. En cada pin que uses, dibujá un cable corto con **`W`**.
3. En la punta del cable poné una etiqueta con **`L`** y escribí el nombre
   exacto de la tabla (por ejemplo `TRIG`).
4. Repetí el mismo nombre en el otro componente.
5. En los pines que no se usan, poné una **X** con **`Q`** (sin conexión).
6. `Herramientas → Anotar esquema`.
7. `Inspeccionar → Comprobación de reglas eléctricas` (ERC).

> La etiqueta tiene que **tocar** el cable. Si queda flotando, no conecta.

---

## 5. Del esquema a la PCB

1. Guardá el esquema (`Ctrl + S`).
2. `Herramientas → Actualizar PCB desde esquema`
   (Tools → Update PCB from Schematic).
3. Aceptar. Se abre **Pcbnew** con las 28 huellas.
4. Acomodá las huellas dentro del contorno de la placa.
5. Dibujá el contorno en la capa `Edge.Cuts`.
6. Ruteá las pistas con **`X`**.
7. `Inspeccionar → Comprobación de reglas de diseño` (DRC).

> Para el informe de la escuela, con el **esquemático** alcanza. La PCB es un
> extra si van a fabricar la placa.

### 5.1. Consejos de ruteo para la v3

- **Audio:** las pistas de `AUDIO_F`, `INL` y `DF_DAC` deben ser **cortas** y
  lejos de las pistas de alimentación. El audio es la parte más sensible.
- **Alimentación:** `+5V` y `GND` con pistas **anchas** (mínimo 1 mm). El
  PAM8403 y el DFPlayer consumen picos altos.
- **C9 470 µF** lo más cerca posible de `VCC`/`GND` del DFPlayer.
- **C1 470 µF** lo más cerca posible de la ESP32-CAM.
- **C4 100 µF** junto al PAM8403.
- **R9** cerca del DFPlayer, no de la ESP32-CAM (protege el módulo).
- **El PIR** conviene dejarlo **lejos** del regulador MT3608 y del PAM8403:
  el calor de esos módulos puede hacerlo disparar solo.

---

## 6. Advertencias sobre las huellas de los módulos

Las huellas de los módulos son **aproximadas** porque cada fabricante hace la
plaquita distinta. Antes de fabricar una PCB:

| Módulo | Qué revisar |
|---|---|
| ESP32-CAM | Medí la separación entre las dos filas (≈22.9 mm). La huella `2x08` no coincide; usá 2 tiras `1x08` o una huella propia. |
| PAM8403 | Contá si tu módulo tiene 8 pines en una fila o 2 filas de 4. |
| **DFPlayer Mini** | Tiene 2 filas de 8 pines a 2.54 mm. **El orden cambia entre clones**: verificá la serigrafía antes de soldar. En el esquema solo se usan 4 pines. |
| **HC-SR501** | 3 pines a 2.54 mm. El orden suele ser `VCC`, `OUT`, `GND`, pero **confirmá con la serigrafía**: si lo conectás al revés, se quema. |
| TP4056 | Verificá si tiene 4 o 6 pads y si es con protección. |
| MT3608 | 4 pads; el orden puede variar. |
| HC-SR04 | 4 pines a 2.54 mm; coincide bien. |
| Portapila 18650 | Verificá el modelo exacto del portapila. |

**Para una maqueta o protoboard no necesitás PCB**: usá tiras de pines hembra y
cables dupont. Las huellas sirven para el informe y para una PCB preliminar.

---

## 7. Si querés crear una huella propia (opcional)

### 7.1. ESP32-CAM

1. `Herramientas → Editor de huellas` (Footprint Editor).
2. `Archivo → Nueva huella`.
3. Nombre: `ESP32CAM_AIThinker`. Biblioteca: creá `SEPDUTR` en la carpeta del
   proyecto.
4. Colocá 2 filas de 8 pads:
   - Herramienta **Pad** (`P`).
   - Tipo: `Through hole` (agujero pasante).
   - Forma: `Circle` o `Oval`.
   - Diámetro del pad: 1.7 mm; agujero: 1.0 mm.
   - Separación entre pines: 2.54 mm.
   - Separación entre filas: la que midas en tu placa (≈22.9 mm).
5. Guardá.
6. En el esquema, doble clic en U1 → Footprint → elegí
   `SEPDUTR:ESP32CAM_AIThinker`.

### 7.2. DFPlayer Mini

Mismo procedimiento, con **2 filas de 8 pads** a 2.54 mm y **15.24 mm entre
filas** (medí tu módulo). Nombre: `DFPLAYER_MINI`.

---

## Resumen

- Las **28 huellas** ya están cargadas en `SEPDUTR_v3_ESP32CAM.kicad_sch`.
- Todas existen en las bibliotecas estándar de **KiCad 7.0**.
- Para verlas: doble clic en un componente → campo **Footprint**, o
  `Herramientas → Asignar huellas`.
- Para pasar a PCB: `Herramientas → Actualizar PCB desde esquema`.
- Las huellas de los módulos son aproximadas: medí tu módulo antes de fabricar.
- **Nuevo en la v3:** U5 (DFPlayer), U6 (PIR), R8, R9, C8, C9.
  **Eliminado:** R3 y LED1 (verde).
