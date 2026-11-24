/*
  ===============================================================================
  S.E.P.D.U.T.R. v3.0
  Sistema Electronico Portatil de Disuasion para Tordos Renegridos
  ESP32-CAM + IA de vision (Edge Impulse) + DFPlayer + PAM8403 + parlante
  ===============================================================================

  QUE CAMBIA RESPECTO DE LA v2
  ---------------------------------------------------------------------------
  La v2 generaba tonos con PWM. Sonaba, pero un tono sintetico no significa
  nada para un ave y se acostumbra en pocos dias. La v3 ataca los tres puntos
  debiles reales:

  1) SONIDO CON SIGNIFICADO
     Se agrega un modulo DFPlayer Mini con tarjeta microSD. Reproduce
     LLAMADOS DE ALARMA REALES del tordo renegrido (grabados) y, como
     complemento, los barridos sinteticos. Un llamado de alarma real es
     muchisimo mas efectivo que un pitido.

  2) DISPARO EN RÁFAGAS, NO CONTINUO
     Lo que sobresalta es el INICIO brusco. La v3 hace rafagas cortas
     (0.9-1.6 s) separadas por silencios aleatorios de 1.2-3.8 s. Eso
     rompe la habituacion mucho mejor que 20 s de barrido.

  3) DETECCION EN TRES ETAPAS
     PIR HC-SR501 (5-7 m, 110 grados) -> HC-SR04 (confirma distancia)
     -> camara + IA (confirma que es tordo). Menos falsos positivos y
     mucho mas alcance que la v2, que solo miraba con el HC-SR04 a 1.5 m.

  Ademas: modo intenso automatico si el ave insiste, volumen y pistas
  aleatorias, y un solo LED de estado (el rojo, que tambien enciende el
  flash blanco integrado).

  ---------------------------------------------------------------------------
  HARDWARE (placa AI-Thinker ESP32-CAM)
  ---------------------------------------------------------------------------
    GPIO12 -> HC-SR04 TRIG              (R7 10k a GND: pin de arranque)
    GPIO2  -> HC-SR04 ECHO              (¡5 V! divisor R1 1k / R2 2k)
    GPIO15 -> pulsador a GND            (INPUT_PULLUP)
    GPIO4  -> LED rojo + flash interno  (R4 220 ohm)
    GPIO14 -> PIR HC-SR501 OUT          (si USAR_PIR = 0: LED verde)
    GPIO13 -> DFPlayer RX por R9 1k     (UART2, solo TX)
              DFPlayer DAC_L -> C8 10uF -> R8 1k -> nodo de audio
              -> PAM8403 INL -> parlante 8 ohm 3 W

    Si USAR_DFPLAYER = 0, GPIO13 vuelve a ser la salida PWM de la v2
    (mismo cableado: R5/R6 10k + C5 2.2nF + C6 10uF). No hay que recablear.

    NO USAR GPIO16 / GPIO17: los ocupa la PSRAM.
    La tarjeta SD del ESP32-CAM NO se usa; sus pines se reutilizan.

  ---------------------------------------------------------------------------
  TARJETA microSD DEL DFPLAYER (formato FAT32)
  ---------------------------------------------------------------------------
    /0001.mp3  Llamado de alarma real del tordo (grabacion 1)
    /0002.mp3  Llamado de alarma real del tordo (grabacion 2)
    /0003.mp3  Llamado de alarma real del tordo (grabacion 3)
    /0004.mp3  Llamado de alarma real del tordo (grabacion 4)
    /0005.mp3  Barrido ascendente 2.2-7.8 kHz
    /0006.mp3  Barrido descendente 7.8-2.2 kHz
    /0007.mp3  Chirps de alarma (6 chirps)
    /0008.mp3  Buzz alternado 3.2/6.5 kHz
    /0009.mp3  Llamado de distress (opcional)

    Los archivos 5-9 se pueden generar con Audacity (Generar -> Tono, o
    Generar -> Chirp) o con cualquier generador de tonos online, y exportar
    como MP3 128 kbps. Duracion sugerida: 1.5-2 s cada uno.

  ---------------------------------------------------------------------------
  ALIMENTACION
  ---------------------------------------------------------------------------
    18650 3.7 V -> TP4056 con proteccion -> MT3608 a 5.0 V
    -> ESP32-CAM, HC-SR04, PAM8403, DFPlayer, PIR
    C1 470uF + C2 100nF junto a la ESP32-CAM.
    C4 100uF junto al PAM8403.  C9 470uF junto al DFPlayer.
    El DFPlayer es sensible a las caidas de tension: sin C9 se reinicia
    o hace ruido al arrancar cada pista.

  ---------------------------------------------------------------------------
  IA CON EDGE IMPULSE
  ---------------------------------------------------------------------------
    1) Entrenar un modelo de IMAGEN 96x96 RGB (MobileNetV1 0.1 o FOMO).
       Clases sugeridas: "tordo", "otra_ave", "fondo".
       Para mas alcance, entrenar a 160x120 y cambiar CAM_FRAMESIZE.
    2) Deployment -> Arduino library -> .zip -> Add .ZIP Library.
    3) Ajustar el include si el header se llama distinto.
    4) Placa "AI Thinker ESP32-CAM", Huge APP, PSRAM Enabled.
    5) Sin el header el sketch compila y funciona solo con PIR + distancia.

  ---------------------------------------------------------------------------
  LOGICA
  ---------------------------------------------------------------------------
    - PIR detecta movimiento (5-7 m). Espera 300 ms.
    - HC-SR04 mide: si el objeto esta a mas de UMBRAL_CM, descarta.
    - Camara + IA: 3 fotos, necesita 2 positivas para confirmar "tordo".
    - Si confirma: rafaga de audio (2-5 pistas, volumen y pausas al azar).
    - Cooldown 25 s normal / 15 s en modo intenso.
    - Modo intenso: si hay 3 eventos en 10 min, sube volumen y rafagas.
      Se apaga solo despues de 30 min sin eventos.
    - Respaldo: cada 40-50 min hace una verificacion.
    - Pulsador: disparo manual (para la demo en clase).
  ===============================================================================
*/

#include <Arduino.h>
#if __has_include(<esp_arduino_version.h>)
  #include <esp_arduino_version.h>
#endif
#include <esp_camera.h>
#include <esp_system.h>
#include <esp_sleep.h>
#include <driver/ledc.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

// ============================================================================
//  CONFIGURACION DE USUARIO
// ============================================================================

// >>> Edge Impulse: si tu proyecto se llama distinto, cambia estas 2 lineas.
#if __has_include("ei_tordo_inferencing.h")
  #include "ei_tordo_inferencing.h"
  #define TIENE_IA 1
#else
  #define TIENE_IA 0
#endif

#define USAR_DFPLAYER       1     // 1 = DFPlayer Mini con llamados reales
                                  // 0 = solo PWM (modo v2, mismo cableado)
#define USAR_PIR            1     // 1 = PIR en GPIO14 (recomendado)
                                  // 0 = LED verde en GPIO14 (modo v2)
#define USAR_DEEP_SLEEP     0     // 1 = ahorro extremo (experimental)

#define UMBRAL_IA           0.60f // confianza minima para "tordo"
#define IA_INTENTOS         3     // fotos por evento
#define IA_NECESARIAS       2     // cuantas deben dar positivo
#define UMBRAL_CM           300   // descarta objetos a mas de 3 m
#define COOLDOWN_MS         25000UL
#define COOLDOWN_INTENSO_MS 15000UL
#define VENTANA_INTENSO_MS  (10UL * 60000UL)
#define RESET_INTENSO_MS    (30UL * 60000UL)
#define RESPALDO_MIN_MIN    40
#define RESPALDO_MAX_MIN    50
#define PIR_CALENTAMIENTO_MS 45000UL
#define SEGUNDOS_ENTRE_CHEQUEOS 5

// Camara: debe coincidir con la entrada del modelo de Edge Impulse.
//   96x96   -> FRAMESIZE_96X96   (modelo chico, alcance ~2 m)
//   160x120 -> FRAMESIZE_QQVGA    (mas alcance, ~3-4 m; reentrenar el modelo)
#define CAM_FRAMESIZE       FRAMESIZE_96X96

// ============================================================================
//  PINES
// ============================================================================
static const int PIN_TRIG  = 12;
static const int PIN_ECHO  = 2;
static const int PIN_BOTON = 15;
static const int PIN_LED   = 4;    // LED rojo + flash integrado

#if USAR_PIR
static const int PIN_PIR   = 14;
#else
static const int PIN_LED_VERDE = 14;
#endif

#if USAR_DFPLAYER
static const int PIN_DF_TX = 13;   // UART2 TX -> R9 1k -> DFPlayer RX
#else
static const int PIN_AUDIO = 13;   // PWM -> R5/R6/C5 -> C6 -> PAM8403 INL
#endif

// ============================================================================
//  AUDIO PWM (solo si NO hay DFPlayer)
// ============================================================================
#if !USAR_DFPLAYER
static const ledc_timer_t   AUDIO_TIMER = LEDC_TIMER_0;
static const ledc_channel_t AUDIO_CHAN  = LEDC_CHANNEL_0;
static const ledc_timer_bit_t AUDIO_RES = LEDC_TIMER_8_BIT;

static const uint32_t F_MIN   = 2500;
static const uint32_t F_MAX   = 7500;
static const uint32_t F_PISO  = 2200;
static const uint32_t F_TECHO = 7800;
#endif

// ============================================================================
//  ESTADO
// ============================================================================
static bool     camaraLista = false;
static bool     iaLista     = false;
static uint32_t ultimoDisparo   = 0;
static uint32_t ultimoIntento   = 0;
static uint32_t proximoRespaldo = 0;
static uint32_t ultimoEvento    = 0;
static uint32_t arranque        = 0;
static uint32_t eventosVentana  = 0;
static uint32_t inicioVentana   = 0;
static uint32_t eventosTotales  = 0;
static bool     modoIntenso     = false;
static uint16_t ultimaPista     = 0;
static bool     pirAnterior     = false;

// ============================================================================
//  CAMARA (AI-Thinker ESP32-CAM)
// ============================================================================
static bool iniciarCamara() {
  camera_config_t config;
  memset(&config, 0, sizeof(config));

  // La camara usa LEDC para XCLK: timer/canal 1 (el audio ya no usa LEDC).
  config.ledc_channel = LEDC_CHANNEL_1;
  config.ledc_timer   = LEDC_TIMER_1;

  config.pin_d0       = 5;
  config.pin_d1       = 18;
  config.pin_d2       = 19;
  config.pin_d3       = 21;
  config.pin_d4       = 36;
  config.pin_d5       = 39;
  config.pin_d6       = 34;
  config.pin_d7       = 35;
  config.pin_xclk     = 0;
  config.pin_pclk     = 22;
  config.pin_vsync    = 25;
  config.pin_href     = 23;
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  config.pin_sccb_sda = 26;
  config.pin_sccb_scl = 27;
#else
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
#endif
  config.pin_pwdn     = 32;
  config.pin_reset    = -1;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB888;
  config.frame_size   = CAM_FRAMESIZE;
  config.jpeg_quality = 12;
  config.fb_count     = 2;                    // 2 buffers: frame mas fresco
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.grab_mode    = CAMERA_GRAB_LATEST;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camara: ERROR 0x%x\n", err);
    return false;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    s->set_brightness(s, 0);
    s->set_contrast(s, 0);
    s->set_saturation(s, 0);
    s->set_whitebal(s, 1);
    s->set_exposure_ctrl(s, 1);
  }
  Serial.println("Camara: OK");
  return true;
}

// ============================================================================
//  INFERENCIA EDGE IMPULSE
// ============================================================================
#if TIENE_IA
static uint8_t *g_img = nullptr;

static int ei_get_data(size_t offset, size_t length, float *out_ptr) {
  for (size_t i = 0; i < length; i++) {
    size_t p = (offset + i) * 3;
    out_ptr[i] = (float)((uint32_t)g_img[p] << 16 |
                         (uint32_t)g_img[p + 1] << 8 |
                         (uint32_t)g_img[p + 2]);
  }
  return 0;
}

static bool etiquetaObjetivo(const char *label) {
  char buf[40];
  size_t n = strlen(label);
  if (n >= sizeof(buf)) n = sizeof(buf) - 1;
  for (size_t i = 0; i < n; i++) buf[i] = (char)tolower((unsigned char)label[i]);
  buf[n] = '\0';

  if (strstr(buf, "otra") || strstr(buf, "no_") || strstr(buf, "fondo") ||
      strstr(buf, "background") || strstr(buf, "negativ")) {
    return false;
  }
  return (strstr(buf, "tordo") != nullptr) || (strcmp(buf, "ave") == 0);
}
#endif

// Devuelve true si el frame actual es "tordo" (o si no hay IA cargada).
static bool analizarFrame() {
#if TIENE_IA
  if (!iaLista || !camaraLista) return true;

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camara: sin frame");
    return false;
  }

  bool objetivo = false;

  if (fb->format == PIXFORMAT_RGB888 &&
      fb->width  == EI_CLASSIFIER_INPUT_WIDTH &&
      fb->height == EI_CLASSIFIER_INPUT_HEIGHT) {

    g_img = fb->buf;

    signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data     = &ei_get_data;

    ei_impulse_result_t result;
    memset(&result, 0, sizeof(result));

    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
    if (err == EI_IMPULSE_OK) {
      Serial.print("IA:");
      for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        Serial.printf(" %s=%.2f", result.classification[i].label,
                      result.classification[i].value);
        if (etiquetaObjetivo(result.classification[i].label) &&
            result.classification[i].value >= UMBRAL_IA) {
          objetivo = true;
        }
      }
      Serial.printf("  (%d ms)\n", (int)result.timing.classification);
    } else {
      Serial.printf("IA: error %d\n", (int)err);
    }
  } else {
    Serial.printf("Frame %ux%u no coincide con el modelo %dx%d\n",
                  (unsigned)fb->width, (unsigned)fb->height,
                  EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
  }

  esp_camera_fb_return(fb);
  return objetivo;
#else
  return true;
#endif
}

// Varias fotos seguidas: exige IA_NECESARIAS positivas de IA_INTENTOS.
static bool confirmarObjetivo() {
#if TIENE_IA
  if (!iaLista || !camaraLista) return true;
  int positivos = 0;
  for (int i = 0; i < IA_INTENTOS; i++) {
    if (analizarFrame()) positivos++;
    if (positivos >= IA_NECESARIAS) return true;
    delay(220);
  }
  return false;
#else
  return true;
#endif
}

// ============================================================================
//  AUDIO: DFPlayer Mini (principal)
// ============================================================================
#if USAR_DFPLAYER

// Trama estandar del DFPlayer: 7E FF 06 CMD 00 PH PL CH CL EF
static void dfFrame(uint8_t cmd, uint16_t param) {
  uint8_t f[10];
  f[0] = 0x7E;
  f[1] = 0xFF;
  f[2] = 0x06;
  f[3] = cmd;
  f[4] = 0x00;
  f[5] = (uint8_t)(param >> 8);
  f[6] = (uint8_t)(param & 0xFF);
  uint16_t suma = 0;
  for (int i = 1; i <= 6; i++) suma += f[i];
  suma = (uint16_t)(-suma);
  f[7] = (uint8_t)(suma >> 8);
  f[8] = (uint8_t)(suma & 0xFF);
  f[9] = 0xEF;
  Serial2.write(f, sizeof(f));
}

static void dfInit() {
  // Solo TX: no necesitamos leer el estado del modulo.
  Serial2.begin(9600, SERIAL_8N1, -1, PIN_DF_TX);
  delay(2000);                 // el DFPlayer tarda en arrancar
  dfFrame(0x06, 24);           // volumen inicial
  delay(100);
  dfFrame(0x16, 0);            // stop
}

static inline void dfVolumen(uint8_t v) { dfFrame(0x06, v); }
static inline void dfPlay(uint16_t pista) { dfFrame(0x03, pista); }
static inline void dfStop() { dfFrame(0x16, 0); }

// Elige una pista evitando repetir la anterior.
static uint16_t elegirPista() {
  uint16_t p = 0;
  for (int i = 0; i < 6; i++) {
    if (random(100) < 65) p = (uint16_t)random(1, 5);   // llamado real
    else                  p = (uint16_t)random(5, 9);   // sintetico
    if (p != ultimaPista) break;
  }
  ultimaPista = p;
  return p;
}

// Rafaga: pistas cortas + silencios aleatorios. Esto es lo que rompe
// la habituacion: nunca suena igual dos veces.
static void reproducirDF() {
  uint8_t vol = modoIntenso ? 30 : (uint8_t)random(24, 29);
  dfVolumen(vol);

  int n = modoIntenso ? 5 : (int)random(2, 5);
  Serial.printf("Audio: %d rafagas, vol %d%s\n", n, (int)vol,
                modoIntenso ? " (intenso)" : "");

  for (int i = 0; i < n; i++) {
    dfPlay(elegirPista());
    delay((unsigned long)random(900, 1600));
    dfStop();
    delay((unsigned long)random(1200, 3800));
  }
  dfStop();
}

static void audioInit() { dfInit(); }
static void audioOff()  { dfStop(); }

#else  // ---------------------- PWM (modo v2) ------------------------------

static void audioInit() {
  ledc_timer_config_t t;
  memset(&t, 0, sizeof(t));
  t.speed_mode      = LEDC_LOW_SPEED_MODE;
  t.duty_resolution = AUDIO_RES;
  t.timer_num       = AUDIO_TIMER;
  t.freq_hz         = 4000;
  t.clk_cfg         = LEDC_AUTO_CLK;
  ledc_timer_config(&t);

  ledc_channel_config_t c;
  memset(&c, 0, sizeof(c));
  c.gpio_num   = PIN_AUDIO;
  c.speed_mode = LEDC_LOW_SPEED_MODE;
  c.channel    = AUDIO_CHAN;
  c.intr_type  = LEDC_INTR_DISABLE;
  c.timer_sel  = AUDIO_TIMER;
  c.duty       = 0;
  c.hpoint     = 0;
  ledc_channel_config(&c);
}

static inline void audioSet(uint32_t f, uint8_t duty) {
  if (f < F_PISO)  f = F_PISO;
  if (f > F_TECHO) f = F_TECHO;
  ledc_set_freq(LEDC_LOW_SPEED_MODE, AUDIO_TIMER, f);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, AUDIO_CHAN, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, AUDIO_CHAN);
}

static inline void audioOff() {
  ledc_set_duty(LEDC_LOW_SPEED_MODE, AUDIO_CHAN, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, AUDIO_CHAN);
}

static void tono(uint32_t f, uint16_t ms, float vib_hz, int vib_amp) {
  uint32_t t0 = millis();
  while (millis() - t0 < ms) {
    float t = (millis() - t0) / 1000.0f;
    int32_t fv = (int32_t)f + (int32_t)(vib_amp * sinf(2.0f * PI * vib_hz * t));
    audioSet((uint32_t)fv, 128);
    delay(6);
  }
}

static void barrido(uint32_t f0, uint32_t f1, uint16_t ms, int pasos = 40) {
  if (pasos < 2) pasos = 2;
  int32_t delta = (int32_t)f1 - (int32_t)f0;
  uint32_t step_us = ((uint32_t)ms * 1000UL) / (uint32_t)pasos;
  for (int i = 0; i <= pasos; i++) {
    int32_t f = (int32_t)f0 + (delta * i) / pasos;
    audioSet((uint32_t)f, 128);
    if (step_us > 10000UL) delay(step_us / 1000UL);
    else                   delayMicroseconds(step_us);
  }
}

static void patronBarrido(bool sube) {
  uint32_t f0 = sube ? F_MIN : F_MAX;
  uint32_t f1 = sube ? F_MAX : F_MIN;
  barrido(f0, f1, 700);
  delay(120);
  barrido(f1, f0, 700);
}

static void patronChirpAlarma() {
  for (int i = 0; i < 5; i++) {
    barrido(F_MAX, 3200, 70, 8);
    audioOff();
    delay(40);
  }
}

static void patronBuzz() {
  uint32_t t0 = millis();
  while (millis() - t0 < 900) {
    tono(3200, 60, 9.0f, 150);
    tono(6500, 60, 9.0f, 150);
  }
}

static void reproducirPWM() {
  int n = modoIntenso ? 5 : (int)random(2, 5);
  for (int i = 0; i < n; i++) {
    switch (random(0, 4)) {
      case 0:  patronBarrido(true);  break;
      case 1:  patronBarrido(false); break;
      case 2:  patronChirpAlarma();  break;
      default: patronBuzz();         break;
    }
    audioOff();
    delay((unsigned long)random(1200, 3800));
  }
  audioOff();
}

#endif  // USAR_DFPLAYER

static void reproducirDisuasor() {
#if USAR_DFPLAYER
  reproducirDF();
#else
  reproducirPWM();
#endif
}

// ============================================================================
//  SENSOR DE DISTANCIA
// ============================================================================
static long medirDistanciaCM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long dur = pulseIn(PIN_ECHO, HIGH, 25000UL);   // ~400 cm maximo
  if (dur == 0) return -1;
  return dur / 58;
}

// ============================================================================
//  ESTADISTICA Y MODO INTENSO
// ============================================================================
static void registrarEvento() {
  uint32_t ahora = millis();
  eventosTotales++;
  ultimoEvento = ahora;

  if (ahora - inicioVentana > VENTANA_INTENSO_MS) {
    inicioVentana  = ahora;
    eventosVentana = 0;
  }
  eventosVentana++;

  if (!modoIntenso && eventosVentana >= 3) {
    modoIntenso = true;
    Serial.println("Modo INTENSO activado (3 eventos en 10 min)");
  }
}

static void actualizarModo() {
  if (modoIntenso && (millis() - ultimoEvento > RESET_INTENSO_MS)) {
    modoIntenso = false;
    eventosVentana = 0;
    Serial.println("Modo intenso desactivado (30 min sin eventos)");
  }
}

// ============================================================================
//  DISPARO
// ============================================================================
static void programarRespaldo() {
  uint32_t minutos = (uint32_t)random(RESPALDO_MIN_MIN, RESPALDO_MAX_MIN + 1);
  proximoRespaldo = millis() + minutos * 60000UL;
}

static void latido() {
  // Parpadeo muy corto cada 10 s: indica "encendido" sin gastar bateria.
  static uint32_t t = 0;
  if (millis() - t > 10000UL) {
    t = millis();
    digitalWrite(PIN_LED, HIGH);
    delay(15);
    digitalWrite(PIN_LED, LOW);
  }
}

static void disparar(const char *origen, bool manual = false,
                     bool sinDistancia = false) {
  uint32_t ahora = millis();
  uint32_t cooldown = modoIntenso ? COOLDOWN_INTENSO_MS : COOLDOWN_MS;

  if (!manual && (int32_t)(ahora - ultimoDisparo) < (int32_t)cooldown) {
    return;
  }
  // Evita ráfagas de fotos si la IA rechaza varias veces seguidas.
  if (!manual && (ahora - ultimoIntento) < 8000UL) {
    return;
  }

  // Etapa 2: distancia. Descarta lo que esta lejos (arboles, personas).
  long d = medirDistanciaCM();
  if (!manual && !sinDistancia) {
    if (d < 0 || d > UMBRAL_CM) {
      Serial.printf("Evento %s descartado: distancia %ld cm\n", origen, d);
      return;
    }
  }

  Serial.printf("Evento %s a %ld cm\n", origen, d);
  ultimoIntento = millis();

  // Etapa 3: IA. Tres fotos, dos positivas.
  if (!confirmarObjetivo()) {
    Serial.println("IA: no es tordo -> no dispara");
    return;
  }

  ultimoDisparo = millis();

  digitalWrite(PIN_LED, HIGH);       // LED rojo + flash interno
  reproducirDisuasor();
  digitalWrite(PIN_LED, LOW);

  registrarEvento();
  programarRespaldo();
}

// ============================================================================
//  SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

#if USAR_PIR
  pinMode(PIN_PIR, INPUT);
#else
  pinMode(PIN_LED_VERDE, OUTPUT);
  digitalWrite(PIN_LED_VERDE, HIGH);
#endif

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_LED, LOW);

  Serial.println();
  Serial.println(F("=============================================="));
  Serial.println(F(" S.E.P.D.U.T.R. v3.0 - ESP32-CAM + IA"));
  Serial.println(F("=============================================="));
  Serial.printf("DFPlayer: %s | PIR: %s | IA: %s\n",
                USAR_DFPLAYER ? "SI" : "NO (PWM)",
                USAR_PIR ? "SI" : "NO (LED verde)",
                TIENE_IA ? "SI" : "NO");

  audioInit();

  randomSeed(esp_random());
  arranque = millis();

  camaraLista = iniciarCamara();

#if TIENE_IA
  iaLista = camaraLista;
  if (iaLista) {
    Serial.printf("IA: %d clases, entrada %dx%d\n",
                  (int)EI_CLASSIFIER_LABEL_COUNT,
                  EI_CLASSIFIER_INPUT_WIDTH,
                  EI_CLASSIFIER_INPUT_HEIGHT);
  }
#else
  Serial.println(F("IA: NO incluida (compila sin Edge Impulse)."));
#endif

  // Tres destellos de arranque.
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH); delay(80);
    digitalWrite(PIN_LED, LOW);  delay(120);
  }

  programarRespaldo();
  ultimoDisparo = millis() - COOLDOWN_MS;

  Serial.println(F("Pines: TRIG=12 ECHO=2 BTN=15 LED=4 PIR=14 DF=13"));
  Serial.println(F("Listo."));
}

// ============================================================================
//  LOOP
// ============================================================================
void loop() {
  // --- Pulsador (antirrebote) ---
  static bool     btnAnterior = HIGH;
  static uint32_t tBtn = 0;
  bool btn = digitalRead(PIN_BOTON);
  if (btn == LOW && btnAnterior == HIGH && (millis() - tBtn) > 300) {
    tBtn = millis();
    disparar("boton", true);
  }
  btnAnterior = btn;

  // --- Etapa 1: PIR ---
#if USAR_PIR
  bool pir = digitalRead(PIN_PIR);
  bool flanco = (pir && !pirAnterior);
  pirAnterior = pir;

  if (flanco && (millis() - arranque > PIR_CALENTAMIENTO_MS)) {
    Serial.println("PIR: movimiento");
    delay(300);                    // deja que el ave se acomode en el cuadro
    disparar("PIR");
  }
#else
  // Sin PIR: el HC-SR04 hace de disparador (alcance corto, como la v2).
  long d = medirDistanciaCM();
  if (d > 0 && d <= UMBRAL_CM) {
    disparar("HC-SR04");
  }
#endif

  // --- Respaldo por tiempo ---
  if ((int32_t)(millis() - proximoRespaldo) >= 0) {
    disparar("respaldo", false, true);
    programarRespaldo();
  }

  actualizarModo();
  latido();

#if USAR_DEEP_SLEEP
  Serial.flush();
#if USAR_PIR
  esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_PIR, 1);
#else
  esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_BOTON, 0);
#endif
  esp_sleep_enable_timer_wakeup((uint64_t)SEGUNDOS_ENTRE_CHEQUEOS * 1000000ULL);
  esp_deep_sleep_start();
#else
  delay(120);
#endif
}
