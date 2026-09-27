# The Heist

This project provides two distinct ways to interact with an OOCSI-enabled IoT device (such as the "Lampo"): a physical ESP32-based hardware button and a mobile-friendly web interface. Both methods send state-change messages over an OOCSI server to toggle the lamp or play Morse code games.

## Features

* **ESP32 Hardware Toggle:** Use a physical push button connected to an ESP32 to remotely control the Lampo.

* **Mobile Web App:** A lightweight, pure HTML/JS web app that lets you control the Lampo from any smartphone browser without needing to install an app.

* **OOCSI Integration:** Utilizes the lightweight OOCSI messaging protocol for low-latency communication over Wi-Fi.

## Prerequisites

### Hardware Requirements

* 1x ESP32 Development Board

* 1x Push Button

* 1x 10k Ohm Resistor (if not using internal pull-down)

* Jumper Wires & Breadboard

* A pre-configured "Lampo" device listening to OOCSI

### Software Requirements

* **Arduino IDE:** To program the ESP32.

* **OOCSI Library for Arduino:** Search for "OOCSI" in the Arduino Library Manager and install it.

* A text editor (or online platform like CodePen/Replit) for the Web App.

## Part 1: ESP32 Hardware Setup

### Wiring

Connect the push button to your ESP32:

* **One side of the button:** Connect to the `3V3` pin.

* **Other side of the button:** Connect to `GPIO 4`. (If relying on hardware pull-down, add a 10k resistor from GPIO 4 to GND).

### Code Configuration

1. Open the `.ino` file in your Arduino IDE.

2. Update your network credentials:

   ```
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   
   ```

3. Update the OOCSI server and unique client name:

   ```
   const char* oocsiServer = "oocsi.id.tue.nl"; // Or "oocsi.net"
   const char* myOOCSIName = "ESP32_Lampo_Controller_123"; // Make this unique!
   
   ```

4. Set the exact OOCSI channel your Lampo is listening to:

   ```
   const char* lampoChannel = "lampo_channel_name";
   
   ```

5. Flash the code to your ESP32. Check the Serial Monitor (115200 baud) to ensure it connects to Wi-Fi and OOCSI.

## Part 2: Mobile Web App Setup

The web app requires zero installation and runs entirely in your phone's browser using OOCSI's WebSockets API.

### Hosting the Web App

1. Create a new `index.html` file or open a new project on a free platform like CodePen, Replit, or GitHub Pages.

2. Paste the provided HTML/JavaScript code.

3. Update the JavaScript line with your Lampo's channel name:

   ```
   OOCSI.send("YOUR_LAMPO_CHANNEL_NAME", { "state": lampoState });
   
   ```

4. Open the hosted URL on your mobile phone.

5. *Optional:* Use "Add to Home Screen" on iOS/Android to treat it like a native app.

## Troubleshooting

* **ESP32 is not connecting to Wi-Fi:** Ensure you are connecting to a 2.4GHz network, as ESP32 boards generally do not support 5GHz networks.

* **Lampo is not responding:** Verify that the OOCSI channel name matches *exactly* between the sender (ESP32/Web) and the receiver (Lampo). Also check the data keys—if Lampo expects `{"power": true}` instead of `{"state": true}`, you must update the variable name in the `oocsi.addBool()` (C++) or `OOCSI.send()` (JS) function.

* **Double-triggering:** If one button press registers as multiple, increase the `delay(200);` debounce time in the ESP32 code.