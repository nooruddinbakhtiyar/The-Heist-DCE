#include <WiFi.h>
#include "OOCSI.h" 

// ==============================
// CONFIGURATION
// ==============================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// OOCSI server address (TU/e default)
const char* OOCSIServer = "oocsi.id.tue.nl";
const char* OOCSIName = "ESP32_Morse_Sender"; // Needs to be unique on the network

// The channel both ESP32, Lampo, and Frontend will use
const char* OOCSIChannel = "morse_escape_room";

// ==============================
// HARDWARE SETUP
// ==============================
const int BUTTON_PIN = 4; // Connect button between GPIO 4 and GND
int lastButtonState = HIGH; // HIGH means not pressed (using pullup)
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; 

OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(115200);

  // Initialize the button pin with an internal pull-up resistor.
  // This means the pin reads HIGH when the button is open, and LOW when pressed.
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Connect to OOCSI
  oocsi.connect(OOCSIName, OOCSIServer, WiFi.localIP());
}

void loop() {
  // Keep OOCSI connection alive
  oocsi.check();

  // Read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON_PIN);

  // Check to see if you just pressed the button
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has changed:
    if (reading != lastButtonState) {
      lastButtonState = reading;

      // Create a new OOCSI message directed to the shared channel
      oocsi.newMessage(OOCSIChannel);

      if (lastButtonState == LOW) {
        // Button is pressed (LOW because of INPUT_PULLUP)
        Serial.println("Button Pressed - Sending to OOCSI");
        oocsi.addString("state", "pressed");
      } else {
        // Button is released
        Serial.println("Button Released - Sending to OOCSI");
        oocsi.addString("state", "released");
      }
      
      // Transmit the message
      oocsi.send();
    }
  }
}
