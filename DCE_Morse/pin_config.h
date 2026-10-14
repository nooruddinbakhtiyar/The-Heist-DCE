#pragma once

#include <Arduino.h>

constexpr uint8_t RED_BUTTON_PIN = 21;

constexpr int BUTTON_ACTIVE_STATE = HIGH;

// Debounce timing (in milliseconds)
constexpr unsigned long DEBOUNCE_DELAY_MS = 25;

// How often the ESP tells the team channel it is alive (ms)
constexpr unsigned long HEARTBEAT_INTERVAL_MS = 10000;
