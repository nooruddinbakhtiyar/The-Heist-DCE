#include <WiFi.h>
#include <OOCSI.h>
#include "pin_config.h" 

// ==========================================
// Network & OOCSI Settings
// ==========================================
const char* ssid = "I like your Skechers";
const char* password = "IRONMAIDEN123";

const char* oocsiServer = "oocsi.id.tue.nl";
const char* oocsiNodeName = "Team_7_Button"; 


const char* oocsiChannel = "Heist_escape_room";    


const char* lampoChannel = "lampo_team_7"; 

OOCSI oocsi = OOCSI();

// ==========================================
// Button State Variables
// ==========================================
int lastButtonState = !BUTTON_ACTIVE_STATE;
int currentButtonState = !BUTTON_ACTIVE_STATE;
unsigned long lastDebounceTime = 0;
bool isCurrentlyPressed = false;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT);

  // 1. Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");

  // 2. Connect to OOCSI
  Serial.println("Connecting to OOCSI...");
  oocsi.connect(oocsiNodeName, oocsiServer, ssid, password);
}

void loop() {
  // Keep OOCSI running and processing incoming messages (if any)
  oocsi.check();

  // Read the physical state of the DFRobot button
  int reading = digitalRead(BUTTON_PIN);

  // Reset debounce timer if the button state changed (due to noise or pressing)
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // Check if the reading has been stable for longer than the debounce delay
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS) {
    
    // If the state has legitimately changed
    if (reading != currentButtonState) {
      currentButtonState = reading;

      // Was it pressed or released?
      if (currentButtonState == BUTTON_ACTIVE_STATE) {
        if (!isCurrentlyPressed) {
          isCurrentlyPressed = true;
          Serial.println("Button Pressed - Broadcasting to App & Lampo");
          
          // 1. Send "pressed" state to the Web Decoder App
          oocsi.newMessage(oocsiChannel);
          oocsi.addString("state", "pressed");
          oocsi.sendMessage();

          // 2. Send ON command to Lampo
          // Note: If Lampo needs color instead of a boolean, change this to addString("color", "white")
          oocsi.newMessage(lampoChannel);
          oocsi.addBool("state", true); 
          oocsi.sendMessage();
        }
      } else {
        if (isCurrentlyPressed) {
          isCurrentlyPressed = false;
          Serial.println("Button Released - Broadcasting to App & Lampo");
          
          // 1. Send "released" state to the Web Decoder App
          oocsi.newMessage(oocsiChannel);
          oocsi.addString("state", "released");
          oocsi.sendMessage();

          // 2. Send OFF command to Lampo
          // Note: If Lampo needs color instead of a boolean, change this to addString("color", "black")
          oocsi.newMessage(lampoChannel);
          oocsi.addBool("state", false); 
          oocsi.sendMessage();
        }
      }
    }
  }

  // Save the reading for the next loop to keep track of state changes
  lastButtonState = reading;
}
