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


* **Lampo is not responding:** Verify that the OOCSI channel name matches *exactly* between the sender (ESP32/Web) and the receiver (Lampo). Also check the data keys—if Lampo expects `{"power": true}` instead of `{"state": true}`, you must update the variable name in the `oocsi.addBool()` (C++) or `OOCSI.send()` (JS) function.

* **Double-triggering:** If one button press registers as multiple, increase the `delay(200);` debounce time in the ESP32 code.
