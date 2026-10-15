#include <WiFi.h>
#include <OOCSI.h>

// ==========================================
// Network settings: SECRET_SSID and SECRET_PASS live in arduino_secrets.h,
// which git ignores so the Wi-Fi password never ends up online
// ==========================================
#if __has_include("arduino_secrets.h")
#include "arduino_secrets.h"
#else
#error "Copy arduino_secrets.h.example to arduino_secrets.h and fill in the Wi-Fi name and password"
#endif

// ==========================================
// OOCSI settings
// ==========================================
const char* const oocsiServer = "oocsi.id.tue.nl";

// Unique name; the server replaces #### with random digits, so a reconnect
// never clashes with the old connection the server has not dropped yet
const char* const oocsiNodeName = "team7_stockings_####";

// THE team channel. Must match HARDWARE_CHANNEL in the Christmas page
// ('OOCSI-things/' + TEAM_SPACE, with TEAM_SPACE = "team7").
const char* const teamChannel = "OOCSI-things/team7";

OOCSI oocsi = OOCSI();

// ==========================================
// ESP32-C6 pins (verified working)
// ==========================================
const int SLIDER_PIN = 0;  // DFRobot Slide Potentiometer -> GPIO 0
const int POT_PIN = 2;     // Rotary Potentiometer -> GPIO 2
const int BUTTON_PIN = 3;  // DFRobot Green Push Button -> GPIO 3

// The page tells players "the knob picks a stocking, the slider fills it".
// Swap these two lines to make the slider pick and the knob fill instead.
const int SELECT_PIN = POT_PIN;
const int FILL_PIN = SLIDER_PIN;

const int TOTAL_STOCKINGS = 8;   // TOTAL_CHANNELS in the page
const int ADC_MAX = 4095;
const int ZONE = (ADC_MAX + 1) / TOTAL_STOCKINGS;  // knob range per stocking
const int ZONE_HYSTERESIS = 60;  // ADC counts past a zone edge before switching

// ==========================================
// Puzzle state
// ==========================================
int lastBarIndex = -1;   // which stocking (0 to 7)
int lastBarHeight = -1;  // fill height (0 to 100)
bool lockedStates[TOTAL_STOCKINGS] = {};

// DFRobot Gravity Button is Active-HIGH
int buttonState = LOW;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// rate limit timer to keep Wi-Fi smooth (10Hz / 100ms)
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 100;

// ==========================================
// Sensors
// ==========================================

// Knob -> stocking 0-7. Every stocking gets an equal slice of the knob, and the
// current one is kept until the knob is clearly past its edge, so ADC noise
// on a boundary does not make the selection flicker.
int readStocking() {
  const int raw = analogRead(SELECT_PIN);
  const int idx = constrain(raw / ZONE, 0, TOTAL_STOCKINGS - 1);
  if (lastBarIndex >= 0 && idx != lastBarIndex) {
    const int low = lastBarIndex * ZONE - ZONE_HYSTERESIS;
    const int high = (lastBarIndex + 1) * ZONE + ZONE_HYSTERESIS;
    if (raw >= low && raw < high) return lastBarIndex;
  }
  return idx;
}

// Slider -> fill height 0-100
int readFill() {
  return constrain(map(analogRead(FILL_PIN), 0, ADC_MAX, 0, 100), 0, 100);
}

// ==========================================
// Messages on the team channel
// ==========================================

// One stocking's state for the Christmas page. No "module" field on purpose:
// the Heist terminal on the same channel ignores messages without one.
void sendStocking(int idx, int hgt, bool locked) {
  oocsi.newMessage(teamChannel);
  oocsi.addInt("bar_index", idx);
  oocsi.addInt("bar_height", hgt);
  oocsi.addBool("is_locked", locked);
  oocsi.sendMessage();

  Serial.printf("Stocking %d | height %d | %s\n", idx, hgt, locked ? "LOCKED" : "UNLOCKED");
}

void setup() {
  Serial.begin(115200);

  pinMode(SLIDER_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  // configure ADC for full 3.3V range
  analogSetAttenuation(ADC_11db);

  // oocsi.connect also connects to Wi-Fi (the ESP32-C6 only sees 2.4 GHz networks)
  Serial.println("Connecting to Wi-Fi and OOCSI...");
  if (oocsi.connect(oocsiNodeName, oocsiServer, SECRET_SSID, SECRET_PASS)) {
    Serial.printf("Connected. Talking on channel: %s\n", teamChannel);
  } else {
    Serial.println("OOCSI connection failed, retrying from loop()");
  }
}

void loop() {
  // keeps the OOCSI connection alive and reconnects if Wi-Fi or OOCSI dropped
  oocsi.keepAlive();
  const unsigned long now = millis();

  // --- 1. Button: fasten / unfasten the selected stocking ---
  const int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = now;
  }
  lastButtonState = reading;

  if (now - lastDebounceTime > debounceDelay && reading != buttonState) {
    buttonState = reading;
    if (buttonState == HIGH) {
      const int idx = lastBarIndex >= 0 ? lastBarIndex : 0;
      lockedStates[idx] = !lockedStates[idx];
      // unfastened: the stocking follows the slider again from where it is now
      if (!lockedStates[idx] || lastBarHeight < 0) lastBarHeight = readFill();
      sendStocking(idx, lastBarHeight, lockedStates[idx]);
    }
  }

  // --- 2. Knob and slider, streamed at most 10 times a second ---
  if (now - lastSendTime >= sendInterval) {
    lastSendTime = now;

    const int currentBarIndex = readStocking();
    const int currentBarHeight = readFill();

    const bool indexChanged = currentBarIndex != lastBarIndex;
    const bool heightChanged = abs(currentBarHeight - lastBarHeight) >= 2;

    if (indexChanged || (!lockedStates[currentBarIndex] && heightChanged)) {
      lastBarIndex = currentBarIndex;
      if (!lockedStates[currentBarIndex]) {
        lastBarHeight = currentBarHeight;
      }
      sendStocking(currentBarIndex, lastBarHeight, lockedStates[currentBarIndex]);
    }
  }
}
