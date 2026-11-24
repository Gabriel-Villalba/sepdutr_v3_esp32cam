# Alcance y efectividad — S.E.P.D.U.T.R. v3

Este documento explica **qué se cambió**, **qué va a pasar cuando funcione** y
**a qué distancia llega**, con números y con las limitaciones reales.

---

## 1. Qué se cambió y por qué

| # | Cambio | Problema que resuelve |
|---|---|---|
| 1 | **DFPlayer Mini + tarjeta microSD** con llamados de alarma reales del tordo | Un tono sintético no significa nada para el ave. Un llamado de alarma real sí. |
| 2 | **Audio en ráfagas** (0,9–1,6 s de sonido + 1,2–3,8 s de silencio, al azar) | El sonido continuo se vuelve “paisaje”. El inicio brusco es lo que sobresalta. |
| 3 | **PIR HC-SR501** como detector de primera etapa (5–7 m, 110°) | La v2 solo detectaba a 1,5 m con el HC-SR04. |
| 4 | **Tres etapas**: PIR → distancia → IA (3 fotos, 2 positivas) | Menos falsos positivos: no dispara por una persona, una hoja o una mascota. |
| 5 | **Modo intenso automático** (3 eventos en 10 min → más volumen y más ráfagas) | Si el ave insiste, el sistema insiste más. Se apaga solo a los 30 min. |
| 6 | **Pistas y volumen aleatorios** + no repetir la misma pista | Rompe la habituación: nunca suena igual dos veces. |
| 7 | **Un solo LED de estado** (rojo + flash) con códigos de parpadeo | El GPIO14 se libera para el PIR. El flash blanco suma un estímulo visual. |
| 8 | **Guardia de 8 s entre intentos** | Evita ráfagas de fotos y ahorra batería cuando la IA rechaza. |

---

## 2. Qué va a pasar, paso a paso

1. El tordo entra en el cono del **PIR** (hasta 5–7 m).
2. El sistema espera 300 ms y mide con el **HC-SR04**.
   - Si está a más de **3 m**, descarta el evento (era un árbol, una persona lejos, etc.).
3. Toma **3 fotos** y las pasa por el modelo de Edge Impulse.
   - Necesita **2 de 3** con “tordo” y confianza ≥ 0,60.
4. Si confirma, enciende el **LED rojo + flash blanco** y reproduce:
   - **2 a 4 ráfagas** (5 en modo intenso),
   - pistas **reales de alarma** (65 %) o sintéticas (35 %),
   - volumen **24–30** sobre 30,
   - silencios aleatorios entre ráfagas.
5. El ave suele reaccionar así: se queda quieta un instante, contesta con su
   propio llamado de alarma, vuela a una rama cercana y **evita la zona** por
   minutos u horas.
6. Si vuelve 3 veces en 10 minutos, entra en **modo intenso**.
7. A los 30 minutos sin eventos, vuelve a modo normal.

**Lo que NO va a pasar**

- No va a eliminar a todos los tordos del barrio.
- No funciona de noche: la cámara no ve sin infrarrojo (el PIR sí, pero no
  confirma especie).
- No distingue un tordo de otra ave si el modelo está mal entrenado.
- No alcanza 30 m de *detección* sin cambiar el sensor.

---

## 3. Distancia: sonido y detección son dos cosas distintas

Esta es la parte que hay que entender bien.

> **El sonido llega lejos. La detección, no tanto.**
> El sistema solo suena cuando detecta. Por eso el **radio útil** es el de
> detección, no el del sonido.

### 3.1. Alcance del sonido (física)

El nivel de presión sonora (SPL) baja **6 dB cada vez que se duplica la
distancia** (campo libre).

Con el PAM8403 a 5 V y un parlante de 8 Ω / 3 W:

- potencia aproximada: **1,5 W RMS**
- sensibilidad típica del parlante: **85 a 90 dB/W a 1 m**
- SPL a 1 m: **87 a 92 dB**

| Distancia | SPL estimado | Qué se percibe |
|---|---|---|
| 1 m | 87–92 dB | Muy fuerte, molesto para una persona |
| 2 m | 81–86 dB | Fuerte |
| 4 m | 75–80 dB | **Umbral de sobresalto para el ave** |
| 8 m | 69–74 dB | Molesto, pero ya no sobresalta |
| 16 m | 63–68 dB | Se oye, molesta poco |
| 32 m | 57–62 dB | Apenas audible en silencio |
| 64 m | 51–56 dB | Se pierde con el ruido ambiente |

**Traducción:**

- **Sobresalto real:** 4 a 8 m.
- **Molestia / disuasión pasiva:** 15 a 25 m.
- **Audible en silencio:** 50 a 80 m.

### 3.2. Alcance de la detección

| Sensor | Alcance típico | Ángulo | Comentario |
|---|---|---|---|
| **PIR HC-SR501** | **5–7 m** (fiable 5 m) | 110° | Detecta cualquier cuerpo caliente, incluida una persona |
| **HC-SR04** | 3–4 m (fiable 3 m) | ~15° | Confirma distancia, no especie |
| **Cámara 96×96 + IA** | **1,5–2,5 m** | ~60° | Un ave de 15 cm ocupa 5–7 píxeles |
| **Cámara 160×120 + IA** | 2,5–4 m | ~60° | Requiere reentrenar el modelo |
| **Cámara 320×240 + FOMO** | 4–7 m | ~60° | Requiere más RAM y un modelo más grande |
| **Barrera IR** (extra) | 10–20 m | línea | Solo en un paso obligado (comederos, nidos) |

**El cuello de botella es la cámara.** A 96×96, a 3 m el ave mide ~5 píxeles:
el modelo no tiene información suficiente. Por eso el sistema confirma a
2–3 m aunque el PIR la vea a 6 m.

### 3.3. Radio útil del sistema completo

| Configuración | Detecta | Confirma | **Radio útil** | Sonido que llega |
|---|---|---|---|---|
| v2 (solo HC-SR04 + cámara) | 1,5 m | 1,5–2 m | **1,5–2 m** | 15–25 m |
| **v3 base** (PIR + distancia + 96×96) | 5–7 m | 2–3 m | **2,5–3 m** | 15–25 m |
| v3 sin IA (solo PIR + distancia) | 5–7 m | 5 m | **5 m** | 15–25 m |
| v3 + modelo 160×120 | 5–7 m | 3–4 m | **3,5–4 m** | 15–25 m |
| v3 + bocina directiva | 5–7 m | 3–4 m | **3,5–4 m** | **30–50 m** |
| v3 + PAM8610 15 W + bocina | 5–7 m | 3–4 m | **3,5–4 m** | **60–100 m** |
| v3 + barrera IR en un paso | 15–20 m | 3–4 m | **15–20 m** | 15–25 m |

> **Conclusión honesta:** la v3 **triplica el radio de disparo** respecto de la
> v2 (de ~1,5 m a ~3 m) y multiplica por 2 o 3 la eficacia del sonido, pero
> **no llega a 30 m de detección** con la cámara actual. Para eso hay que
> cambiar el sensor de detección, no el audio.

---

## 4. Cómo aumentar el alcance (de mayor a menor impacto)

### 4.1. Bocina o concentrador acústico — gratis o casi

Poné el parlante dentro de un cono, embudo o tubo de PVC abierto. Concentra el
sonido en un haz en lugar de repartirlo en todas direcciones.

- Ganancia típica: **+6 a +10 dB** en el eje.
- Efecto: **duplica** la distancia de sobresalto (de 4–8 m a 8–16 m).
- Contra: el haz es angosto; hay que apuntarlo.

### 4.2. Modelo de IA a 160×120

Cambiá `CAM_FRAMESIZE` a `FRAMESIZE_QQVGA` y reentrená el modelo a 160×120.

- Ganancia: **+1 a +1,5 m** de confirmación.
- Costo: más tiempo de inferencia (~1,5–2 s por foto).

### 4.3. Amplificador más potente + bocina

Reemplazar el PAM8403 por un **PAM8610 (15 W + 15 W)** o **TPA3116** y usar una
bocina de 10–15 W.

- Ganancia: **+10 dB** sobre el PAM8403 (4× la potencia) y +10 dB de la bocina.
- Efecto: **4× la distancia** (sobresalto a 16–30 m).
- Contra: consume 1–2 A; la batería 18650 dura mucho menos. Necesita fuente de
  5 V/3 A y cables gruesos.

### 4.4. Barrera infrarroja en un paso obligado

Un emisor y receptor IR a 10–20 m, cruzando el comedero o la entrada del nido.

- Ganancia: detección a **15–20 m**, pero solo en una línea.
- Es la única forma barata de detectar lejos con este hardware.

### 4.5. Mover el equipo cada 3–5 días

No es una mejora de hardware, pero es la medida **más eficaz** contra la
habituación. Cambiar de lugar y de horario multiplica la vida útil del efecto.

---

## 5. Nota realista: de 1 a 10

| Escenario | Nota |
|---|---|
| v2 (tonos PWM) | 5,5–6,5 / 10 el primer día, 4 / 10 a las 3 semanas |
| **v3 (llamados reales + ráfagas + PIR)** | **7,5–8 / 10** la primera semana |
| v3 a las 3 semanas | 5,5–6 / 10 |
| v3 + bocina + reubicación periódica | **7 / 10 sostenido** |
| v3 + exclusión física (malla) | 9,5 / 10 |

La exclusión física (malla de menos de 3,5 cm alrededor del nido) sigue siendo
el único método realmente definitivo. El sonido es el complemento tecnológico,
no el reemplazo.

---

## 6. Cómo medir si funciona (para el informe)

1. **Línea base (3 días):** contá cuántas veces aparece el tordo por hora, sin
   encender el equipo. Anotá hora y distancia.
2. **Con el equipo (7 días):** misma planilla, con el equipo encendido.
3. **Comparación:** cantidad de visitas por hora y tiempo de permanencia.
4. **Registrar en el Monitor Serie:** el código imprime cada evento, la
   distancia, el resultado de la IA y el modo (normal/intenso).

Con esos datos podés afirmar algo medible, en lugar de “funciona” o “no
funciona”.

---

## 7. Seguridad y convivencia

- No apuntes el parlante a una casa vecina ni lo dejes sonando de noche.
- Mantené el nivel por debajo de **90 dB a 1 m** en zonas con personas.
- El objetivo es **espantar**, no dañar al ave. Un sonido fuerte y breve es
  suficiente; el estrés prolongado no es defendible en un trabajo escolar.
- Revisá la normativa de tu municipio sobre ruidos y sobre manejo de fauna.
