# Arduino-Keypad-OLED-and-Modes
<img width="500" src="https://github.com/Gabe-H/Arduino-Keypad-OLED-and-Modes/blob/master/images/IMG_1426.jpg?raw=true" alt="example"></img>
<br>
A simple macro keypad with preset mixing.<br>
<br>
I came up with the idea after the first time I made a keypad with multiple modes, but I never managed to fill all of the 9 buttons with practical macros, and was constantly switching from mode to mode. This new keypad allows you to have multiple modes on a single pad, separated by each row. A rotary encoder will  quickly and easily change each row's mode.<br>
With all keyboard functions being stored on the Arduino, there is not host software, and it will be plug and play on any computer.

## Default examples
The program comes ready for a 5x2 keypad, 128x64 SSD1306 OLED (yellow & blue), and rotary encoder.<br>
The default modes are:<br>

### Discord (custom hotkeys in settings):
```
F13/Mute, F14/Deafen, F15/Streamer Mode, F16/Toggle screen share, F17/(Actually used for Zoom)
```

### Media:
```
-15s, Previous, Play/Pause, Next, +15s
```

### Volume:
```
Vol-, Vol+, Toggle Mute, Vol-, Vol+
```

### F Keys
```
F1, F5, F10, F11, F12
```

## Requirements
- PlatformIO
- Arduino Leonard, Pro Micro or other ATMEGA32u4 board
- SSD1306
- buttons wired by rows and columns (default 5x2)
- Rotary encoder

## Code setup
Use `src/config.h` to customize/setup the keypad. <br>.

Be most concerned about the Knob and keypad pinouts, then `NUM_MODES`, `modeNames` and `buttonType` / `keyboardButtons` / `consumerButtons` for customization.<br>
For further customization and details, the full `config.h` is:
- `SCREENSAVER_TIMEOUT` - time before the edit screen changes to show the active modes
- `ENCODER_AS_VOLUME` - Uncommented, this will make the encoder change volume while the board is idling (screensaver mode). Press the encoder button to activate edit mode
- `DEBUG_SERIAL` - Uncomment for debugging, but leave commented out for performance.
- `EEPROM_INIT` - Uncomment to reset the EEPROM that will be used later. Sometimes needed if the board starts in a bad mode. WILL RESET EVERY BOOT
- `KNOB` pinouts: Button, Clock, Data to Arduino pins (default A0, 15, 14 respectively)
- `OLED` dimensions and I2C address
- Keypad dimensions - then pins for each `ROW`/`COL`
- `keys` - only change to unique characters in the same dimensions as your keypad
- `NUM_MODES` - number of modes to be used
- `modeNames` - Name of each mode, MAX 10 characters to fit on the OLED screen
- `buttonType` - Use `KEY` / `CON` to identify each bind as a Keyboard or Consumer button (Rule of thumb: ASCII and F keys will be Keyboard (KEY), most media/function keys will be Consumer (CON)) or ignore a key with `UNUSED`
- `keyboardButtons`/`consumerButtons` - 2 arrays to differentiate between Keyboard and Consumer keys. If key is part of Keyboard library, define K in `buttonType` or C for Consumer library keys. For blank keys, define in `buttonType` that the key is `UNUSED`.
