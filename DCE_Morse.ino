#include <WiFi.h>
#include <OOCSI.h>
#include "pin_config.h"

// ==========================================
// Network settings  (fill in before flashing, never upload to Data Foundry)
// ==========================================
const char* ssid     = "iotroam";
const char* password = "ESPsuitcaseTeam7";

// ==========================================
// OOCSI settings
// ==========================================
const char* oocsiServer = "oocsi.id.tue.nl";

// Unique name; the server replaces #### with random digits
const char* oocsiNodeName = "team7_suitcase_####";

// THE team channel. Must be identical in index.html (TEAM_CHANNEL).
const char* teamChannel = "OOCSI-things/team7";

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

void sendRed(const char* state, long durationMs) {
  oocsi.newMessage(teamChannel);
  oocsi.addString("module", "morse_button");
  oocsi.addString("state", state);
  // Lampo (OOCSI Things): on while pressed, off when released
  oocsi.addBool("lampo_toggle", strcmp(state, "pressed") == 0);
  if (durationMs >= 0) oocsi.addInt("duration_ms", (int)durationMs); // only on release
  oocsi.sendMessage();
}

void setup() {
  Serial.begin(115200);
  pinMode(RED_BUTTON_PIN, INPUT_PULLDOWN);  // DFRobot module: HIGH when pressed

  // oocsi.connect also connects to Wi-Fi
  Serial.println("Connecting to Wi-Fi and OOCSI...");
  oocsi.connect(oocsiNodeName, oocsiServer, ssid, password);

  Serial.print("Connected. Talking on channel: ");
  Serial.println(teamChannel);
  sendHeartbeat();
  lastHeartbeat = millis();
}

void loop() {
  // keeps the OOCSI connection alive
  oocsi.check();

  // --- Red button: debounce, then send pressed / released ---
  int reading = digitalRead(RED_BUTTON_PIN);
  if (reading != lastReading) {
    lastChange = millis();
  }
  lastReading = reading;

  if ((millis() - lastChange) > DEBOUNCE_DELAY_MS && reading != stableState) {
    stableState = reading;

    if (stableState == BUTTON_ACTIVE_STATE) {
      pressedAt = millis();
      Serial.println("Red pressed  -> lamp ON");
      sendRed("pressed", -1);
    } else {
      long held = millis() - pressedAt;
      Serial.print("Red released -> lamp OFF (held ");
      Serial.print(held);
      Serial.println(" ms)");
      sendRed("released", held);
    }
  }

  // --- Heartbeat so the web page can show the suitcase is online ---
  if (millis() - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
    lastHeartbeat = millis();
    sendHeartbeat();
  }
}
