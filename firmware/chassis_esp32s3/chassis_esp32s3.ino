// 6x6 chassis - first test
// HOTRC DS-600 receiver (F-06A) -> ESP32-S3-DevKitC-1 -> 6x BLDC hall controllers (3 left, 3 right)
// Skid steer: CH1 = steering wheel, CH2 = throttle trigger, CH3 = ARM switch/button.
// ESP32-S3 has NO DAC -> speed is 20 kHz PWM + RC filter (4.7k + 4.7uF) -> controller speed input (0..3.3 V).
// RC in:     GPIO4 = CH1, GPIO5 = CH2, GPIO6 = CH3
// Speed out: GPIO7 left, GPIO15 right (PWM -> filter)
// DIR out:   GPIO16 left, GPIO17 right -> 2N7000 gate -> pulls controller DIR pins to GND
// All 7 pins are next to each other on the left header. Needs Arduino-ESP32 core 3.x.
//
// Safety: wheels OFF the ground for the first runs. MAX_OUT limits top speed.

#include <Arduino.h>

// ---------- pins ----------
const int PIN_CH1 = 4;    // steering
const int PIN_CH2 = 5;    // throttle
const int PIN_CH3 = 6;    // arm switch
const int PIN_SPD_L = 7;  // PWM left
const int PIN_SPD_R = 15; // PWM right
const int PIN_DIR_L = 16;
const int PIN_DIR_R = 17;
const int PWM_FREQ = 20000, PWM_BITS = 10;

// ---------- tuning ----------
const float MAX_OUT    = 0.40f; // 0..1, fraction of usable throttle range (start low!)
const float V_START    = 0.80f; // throttle voltage where controllers start to turn (measure with a pot)
const float V_TOP      = 3.20f; // max filtered PWM voltage
const float RAMP_PER_S = 0.8f;  // how fast speed may change (full scale per second)
const float DEADBAND   = 0.06f; // stick/trigger centre deadband
const float STEER_GAIN = 0.7f;  // how strong turning is
const bool  INVERT_L   = false; // flip if left side runs backwards
const bool  INVERT_R   = true;  // right side is mirrored, usually needs inverting
const uint32_t SIGNAL_TIMEOUT_MS = 100; // no pulses for this long = failsafe stop
const uint32_t REVERSE_PAUSE_MS  = 250; // wait at zero before flipping DIR

// ---------- RC input capture ----------
const int chPins[3] = {PIN_CH1, PIN_CH2, PIN_CH3};
volatile uint32_t riseUs[3]  = {0, 0, 0};
volatile uint32_t widthUs[3] = {1500, 1500, 1000};
volatile uint32_t lastMs[3]  = {0, 0, 0};

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
  float v = (s.cur < 0.02f) ? 0.0f : V_START + s.cur * MAX_OUT * (V_TOP - V_START);
  ledcWrite(s.spdPin, (uint32_t)(v / 3.3f * 1023.0f));
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
  ledcAttach(PIN_SPD_L, PWM_FREQ, PWM_BITS);
  ledcAttach(PIN_SPD_R, PWM_FREQ, PWM_BITS);
  hardStop();
  for (int i = 0; i < 3; i++) pinMode(chPins[i], INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CH1), isrCh<0>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CH2), isrCh<1>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CH3), isrCh<2>, CHANGE);
  Serial.println("6x6 first test ready. Flip ARM switch with throttle centred.");
}

void loop() {
  static uint32_t lastLoop = millis(), lastPrint = 0;
  uint32_t now = millis();
  float dt = (now - lastLoop) / 1000.0f; lastLoop = now;

  noInterrupts();
  uint32_t w1 = widthUs[0], w2 = widthUs[1], w3 = widthUs[2];
  uint32_t l1 = lastMs[0],  l2 = lastMs[1];
  interrupts();

  bool signalOk = (now - l1 < SIGNAL_TIMEOUT_MS) && (now - l2 < SIGNAL_TIMEOUT_MS);
  float steer = norm(w1), thr = norm(w2);
  bool armSw = (w3 > 1700) && (now - lastMs[2] < SIGNAL_TIMEOUT_MS);

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
    Serial.printf("CH1 %4u CH2 %4u CH3 %4u | sig %d arm %d | L %+.2f R %+.2f\n",
                  w1, w2, w3, signalOk, armed, L.cur * L.dirState, R.cur * R.dirState);
  }
  delay(10);
}
