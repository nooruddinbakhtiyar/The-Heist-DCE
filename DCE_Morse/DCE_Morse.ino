#include <WiFi.h>
#include <OOCSI.h>
#include "pin_config.h"

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

// Unique name; the server replaces #### with random digits
const char* const oocsiNodeName = "team7_suitcase_####";

// THE team channel. Must be identical in index.html (TEAM_CHANNEL).
const char* const teamChannel = "OOCSI-things/team7";

OOCSI oocsi = OOCSI();

// ==========================================
// Red button state (debounced)
// ==========================================
int lastReading = !BUTTON_ACTIVE_STATE;
int stableState = !BUTTON_ACTIVE_STATE;
unsigned long lastChange = 0;
unsigned long pressedAt = 0;
unsigned long lastHeartbeat = 0;

// ==========================================
// Messages on the team channel
// ==========================================
void sendHeartbeat() {
  oocsi.newMessage(teamChannel);
  oocsi.addString("module", "suitcase");
  oocsi.addString("event", "online");
  oocsi.addInt("uptime_s", (int)(millis() / 1000));
  oocsi.sendMessage();
}

void sendRed(bool pressed, unsigned long heldMs) {
  oocsi.newMessage(teamChannel);
  oocsi.addString("module", "morse_button");
  oocsi.addString("state", pressed ? "pressed" : "released");
  // Lampo (OOCSI Things): on while pressed, off when released
  oocsi.addBool("lampo_toggle", pressed);
  if (!pressed) oocsi.addInt("duration_ms", (int)heldMs);
  oocsi.sendMessage();
}

void setup() {
  Serial.begin(115200);
  pinMode(RED_BUTTON_PIN, INPUT_PULLDOWN);  // DFRobot module: HIGH when pressed

  // oocsi.connect also connects to Wi-Fi
  Serial.println("Connecting to Wi-Fi and OOCSI...");
  oocsi.connect(oocsiNodeName, oocsiServer, SECRET_SSID, SECRET_PASS);

  Serial.printf("Talking on channel: %s\n", teamChannel);
  sendHeartbeat();
  lastHeartbeat = millis();
}

void loop() {
  // keeps the OOCSI connection alive
  oocsi.check();
  const unsigned long now = millis();

  // --- Red button: debounce, then send pressed / released ---
  const int reading = digitalRead(RED_BUTTON_PIN);
  if (reading != lastReading) {
    lastReading = reading;
    lastChange = now;
  }

  if (now - lastChange > DEBOUNCE_DELAY_MS && reading != stableState) {
    stableState = reading;

    if (stableState == BUTTON_ACTIVE_STATE) {
      pressedAt = now;
      Serial.println("Red pressed  -> lamp ON");
      sendRed(true, 0);
    } else {
      const unsigned long held = now - pressedAt;
      Serial.printf("Red released -> lamp OFF (held %lu ms)\n", held);
      sendRed(false, held);
    }
  }

  // --- Heartbeat so the web page can show the suitcase is online ---
  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
    lastHeartbeat = now;
    sendHeartbeat();
  }

  // let the CPU idle between polls; 1 ms is far below the debounce time
  delay(1);
}
