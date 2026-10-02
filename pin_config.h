#pragma once

// ==========================================
// DFRobot DFR0029 Digital Push Button V3 (red)
// Wiring with female-male jumpers:
//   green  (signal) -> GPIO 21
//   red    (+)      -> 3V3   (NOT 5V)
//   black  (-)      -> G / GND
// The module outputs HIGH when pressed.
// ==========================================

#define RED_BUTTON_PIN  21

#define BUTTON_ACTIVE_STATE HIGH

// Debounce timing (in milliseconds)
#define DEBOUNCE_DELAY_MS 25

// How often the ESP tells the team channel it is alive (ms)
#define HEARTBEAT_INTERVAL_MS 10000
