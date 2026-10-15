# The Heist

Team 7's Morse-code puzzle. A red push button in the suitcase (ESP32-C6) sends every press and release over OOCSI. The web terminal shows them as flashes of a lamp ("Lampo") behind a hole in a wall; players decode the Morse code and type it into the terminal to unlock the vault.

## Files

- `DCE_Morse/`: Arduino sketch for the suitcase ESP32
- `DCE_Stockings/`: Arduino sketch for the stockings ESP32 of Dream 2 (Christmas)
- `index.html`: the web terminal (one file, wall picture included)
- `schematic.jpeg`: button wiring (signal to GPIO 21, + to 3V3, - to GND)

## Flashing the suitcase

1. Install the ESP32 boards and the **OOCSI** library in the Arduino IDE.
2. Copy `DCE_Morse/arduino_secrets.h.example` to `DCE_Morse/arduino_secrets.h` and fill in the Wi-Fi password. Git ignores this file, so the password stays off GitHub.
3. Open `DCE_Morse/DCE_Morse.ino`, select the ESP32-C6 board and upload. The Serial Monitor (115200 baud) shows every press and release.

The stockings ESP32 is flashed the same way from `DCE_Stockings/` (its own `arduino_secrets.h`). The knob picks a stocking, the slider fills it and the green button fastens it, as the Christmas page tells players; swap `SELECT_PIN` and `FILL_PIN` at the top of the sketch to turn that around.

## Terminal

`TEAM_NUMBER` and `CORRECT_PASSCODE` are at the top of the `<script>` in `index.html`. The page joins the team's OOCSI Things teamspace as `heist-terminal`, with outputs `solved` and `lamp`.

## Messages on `OOCSI-things/team7`

| Sender | When | Data |
| --- | --- | --- |
| Suitcase | every 10 s | `module: "suitcase", event: "online", uptime_s` |
| Suitcase | button pressed | `module: "morse_button", state: "pressed", lampo_toggle: true` |
| Suitcase | button released | `module: "morse_button", state: "released", lampo_toggle: false, duration_ms` |
| Terminal | page connected | `module: "terminal", event: "online", team: 7` |
| Terminal | correct passcode | `module: "terminal", event: "solved", team: 7` |
| Stockings | knob, slider or button changes | `bar_index` (0-7), `bar_height` (0-100), `is_locked` |

The stockings messages have no `module` field, so the terminal ignores them; the Christmas page reads them.

The channel is set in `DCE_Morse.ino` and `DCE_Stockings.ino` (`teamChannel`), in `index.html` (`TEAM_NUMBER`) and in the Christmas page (`TEAM_SPACE`); keep them the same.

Claude and Gemini were used for the comments within the files
