#include <OOCSI.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* oocsiServer = "oocsi.id.tue.nl"; 
const char* myOOCSIName = "ESP32_Lampo_Controller"; //

OOCSI oocsi = OOCSI();

const int buttonPin = 4; 
int lastButtonState = LOW;


bool lampoState = false; 

//Channel name
const char* lampoChannel = "lampo_channel_name"; 

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLDOWN); 

  oocsi.connect(myOOCSIName, oocsiServer, ssid, password);
}

void loop() {
  oocsi.check(); 

  int currentButtonState = digitalRead(buttonPin);

  // Trigger from high to low
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    
    // flip state
    lampoState = !lampoState; 
    
    oocsi.newMessage(lampoChannel);
    
    // "state" for different variable
    oocsi.addBool("state", lampoState);
    
    
    
    oocsi.sendMessage();
    
    Serial.print("Button Pressed - Lampo toggled to: ");
    Serial.println(lampoState ? "ON" : "OFF");
    
    // preventing double clicks
    delay(200); 
  }
  
  lastButtonState = currentButtonState;
}