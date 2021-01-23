# Arduino-Keypad-OLED-and-Modes
<img width="500" src="https://github.com/Gabe-H/Arduino-Keypad-OLED-and-Modes/blob/master/images/IMG_1426.jpg?raw=true" alt="example"></img>
<br>
This keypad program brings a minimal, quickly customizable macro pad.<br>
I came up with the idea after I made a first keypad with modes, but I never managed to fill every button with practical macros, and was constantly switching from mode to mode. This keypad allows you to have multiple modes on a single pad, separated by each row. A rotary encoder will (by default) change volume while idling, but press it and you can quickly easily change each row's mode.<br>
With all keyboard functions being stored on the Arduino, there is not host software, and it will be plug and play on any computer.

## Default examples
The program comes ready for a 5x2 keypad, 128x64 SSD1306 OLED (yellow & blue), and standard rotary encoder (I don't know the id).<br>
The default modes are:<br>

### Discord (custom hotkeys in settings):
```
F13/Mute, F14/Deafen, F15/Streamer Mode, F16/Toggle screen share, F17/Unused
```

### Media:
```
-15s, Previous, Play/Pause, Next, +15s
```

### Volume:
```
Vol-, Vol+, Toggle Mute, Vol-, Vol+
```

### Numbers:
```
1, 2, 3, 4, 5
```

### Lower F:
```
F1, F2, F3, F4, F5
```

### Upper F:
```
F8, F9, F10, F11, F12
```

## Requirements
- PlatformIO
- Arduino Leonard, Pro Micro or other ATMEGA32u4 board
- SSD1306
- buttons wired by rows and columns (default 5x2)
- OPTIONAL case such as <a href="https://www.thingiverse.com/thing:4539723">Codek</a>

## Code setup
1. Set the number of modes you want, title them, and setup your keypad. NOTE: It is not recommended to have more than about 10 modes
2. Setup your keybinds in `determineKey()`. Each case belongs to your modes in order
3. If you don't want the encoder to change volume while idle, you can change that option at the top of the code

## Usage
Press each key to activate it's keybind for the current mode. To change modes, press the encoder button (or rotate if encoder volume is disabled), and twist to change each row's mode. Click the encoder button to cycle through each row to edit. The keys will be ready instantly, but the screensaver will appear after default 2500ms.
