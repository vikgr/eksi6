// 6x6 chassis - first test
// HOTRC DS-600 receiver (F-06A) -> ESP32 DevKit -> 6x BLDC hall controllers (3 left, 3 right)
// Skid steer: CH1 = steering wheel, CH2 = throttle trigger, CH3 = ARM switch/button, CH4 = turtle/rabbit.
// Speed out: ESP32 DAC (GPIO25 left, GPIO26 right) -> controller speed/VR/throttle input (0..3.3 V).
// DIR out:   GPIO27 left, GPIO14 right -> 2N7000 gate -> pulls controller DIR pins to GND.
//
// Safety: wheels OFF the ground for the first runs. MAX_OUT limits top speed.

#include <Arduino.h>

// ---------- pins ----------
const int PIN_CH1 = 34;   // steering   (input-only pin, fine)
const int PIN_CH2 = 35;   // throttle
const int PIN_CH3 = 32;   // arm switch
const int PIN_CH4 = 33;   // turtle / rabbit switch
const int PIN_SPD_L = 25; // DAC1
const int PIN_SPD_R = 26; // DAC2
const int PIN_DIR_L = 27;
const int PIN_DIR_R = 14;

// ---------- tuning ----------
// Turtle / rabbit (CH4 switch). Fractions of the usable throttle range.
// Calibrate on stands: measure wheel rpm, 6.5" wheel: 2.5 km/h ~ 80 rpm, 5 km/h ~ 160 rpm.
const float MAX_OUT_TURTLE = 0.15f; // target ~2.5 km/h
const float MAX_OUT_RABBIT = 0.30f; // target ~5 km/h, never above 5 km/h
float maxOut = MAX_OUT_TURTLE;      // starts in turtle
const float V_START    = 0.80f; // throttle voltage where controllers start to turn (measure with a pot)
const float V_TOP      = 3.20f; // max DAC voltage
const float RAMP_PER_S = 0.8f;  // how fast speed may change (full scale per second)
const float DEADBAND   = 0.06f; // stick/trigger centre deadband
const float STEER_GAIN = 0.7f;  // how strong turning is
const bool  INVERT_L   = false; // flip if left side runs backwards
const bool  INVERT_R   = true;  // right side is mirrored, usually needs inverting
const uint32_t SIGNAL_TIMEOUT_MS = 100; // no pulses for this long = failsafe stop
const uint32_t REVERSE_PAUSE_MS  = 250; // wait at zero before flipping DIR

// ---------- RC input capture ----------
const int chPins[4] = {PIN_CH1, PIN_CH2, PIN_CH3, PIN_CH4};
volatile uint32_t riseUs[4]  = {0, 0, 0, 0};
volatile uint32_t widthUs[4] = {1500, 1500, 1000, 1000};
volatile uint32_t lastMs[4]  = {0, 0, 0, 0};

template <int i>
void IRAM_ATTR isrCh() {
  uint32_t t = micros();
  if (digitalRead(chPins[i])) {
    riseUs[i] = t;
  } else {
    uint32_t w = t - riseUs[i];
    if (w > 800 && w < 2200) { widthUs[i] = w; lastMs[i] = millis(); }
  }
}

float norm(uint32_t w) {               // 1000..2000 us -> -1..+1
  float v = ((int)w - 1500) / 500.0f;
  if (v > 1) v = 1; if (v < -1) v = -1;
  if (fabsf(v) < DEADBAND) return 0;
  return v;
}

// ---------- one side of the chassis (3 controllers in parallel) ----------
struct Side {
  int spdPin, dirPin; bool inv;
  float cur = 0;            // current magnitude 0..1
  int8_t dirState = 1;      // +1 forward, -1 reverse
  uint32_t lastNonZero = 0;
};
Side L{PIN_SPD_L, PIN_DIR_L, INVERT_L};
Side R{PIN_SPD_R, PIN_DIR_R, INVERT_R};

void writeSide(Side &s) {
  bool rev = (s.dirState < 0) ^ s.inv;
  digitalWrite(s.dirPin, rev ? HIGH : LOW);   // HIGH -> transistor on -> DIR to GND
  float v = (s.cur < 0.02f) ? 0.0f : V_START + s.cur * maxOut * (V_TOP - V_START);
  dacWrite(s.spdPin, (int)(v / 3.3f * 255.0f));
}

void updateSide(Side &s, float target, float dt) {
  uint32_t now = millis();
  int8_t want = (target > 0) - (target < 0);
  float tm = fabsf(target);
  if (want != 0 && want != s.dirState) {      // reversal requested: stop first, pause, then flip
    tm = 0;
    if (s.cur == 0 && now - s.lastNonZero >= REVERSE_PAUSE_MS) s.dirState = want;
  }
  float step = RAMP_PER_S * dt;
  if (s.cur < tm) s.cur = min(tm, s.cur + step);
  else            s.cur = max(tm, s.cur - step);
  if (s.cur > 0) s.lastNonZero = now;
  writeSide(s);
}

void hardStop() {
  L.cur = R.cur = 0;
  L.lastNonZero = R.lastNonZero = millis();
  writeSide(L); writeSide(R);
}

bool armed = false;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_DIR_L, OUTPUT); pinMode(PIN_DIR_R, OUTPUT);
  hardStop();
  for (int i = 0; i < 4; i++) pinMode(chPins[i], INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CH1), isrCh<0>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CH2), isrCh<1>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CH3), isrCh<2>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CH4), isrCh<3>, CHANGE);
  Serial.println("6x6 first test ready. Flip ARM switch with throttle centred.");
}

void loop() {
  static uint32_t lastLoop = millis(), lastPrint = 0;
  uint32_t now = millis();
  float dt = (now - lastLoop) / 1000.0f; lastLoop = now;

  noInterrupts();
  uint32_t w1 = widthUs[0], w2 = widthUs[1], w3 = widthUs[2], w4 = widthUs[3];
  uint32_t l1 = lastMs[0],  l2 = lastMs[1];
  interrupts();

  bool signalOk = (now - l1 < SIGNAL_TIMEOUT_MS) && (now - l2 < SIGNAL_TIMEOUT_MS);
  float steer = norm(w1), thr = norm(w2);
  bool armSw = (w3 > 1700) && (now - lastMs[2] < SIGNAL_TIMEOUT_MS);
  bool rabbit = (w4 > 1700) && (now - lastMs[3] < SIGNAL_TIMEOUT_MS);
  float maxTarget = rabbit ? MAX_OUT_RABBIT : MAX_OUT_TURTLE;
  float mStep = 0.2f * dt;                             // gear change takes ~1 s, no jerk
  if (maxOut < maxTarget) maxOut = min(maxTarget, maxOut + mStep);
  else                    maxOut = max(maxTarget, maxOut - mStep);

  if (!armSw || !signalOk) armed = false;
  else if (!armed && thr == 0) armed = true;   // arm only with throttle centred

  if (!armed) {
    hardStop();
  } else {
    float left  = thr + steer * STEER_GAIN;
    float right = thr - steer * STEER_GAIN;
    float m = max(fabsf(left), fabsf(right));
    if (m > 1) { left /= m; right /= m; }
    updateSide(L, left, dt);
    updateSide(R, right, dt);
  }

  if (now - lastPrint > 200) {
    lastPrint = now;
    Serial.printf("CH1 %4u CH2 %4u CH3 %4u CH4 %4u | sig %d arm %d %s | L %+.2f R %+.2f\n",
                  w1, w2, w3, w4, signalOk, armed, rabbit ? "RABBIT" : "turtle", L.cur * L.dirState, R.cur * R.dirState);
  }
  delay(10);
}
