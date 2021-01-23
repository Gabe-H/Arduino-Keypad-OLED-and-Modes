# Arduino-Keypad-OLED-and-Modes
<img width="500" src="https://github.com/Gabe-H/Arduino-Keypad-OLED-and-Modes/blob/master/images/IMG_1426.jpg?raw=true" alt="example"></img>
## Requirements
- PlatformIO
- Arduino Leonard, Pro Micro or other ATMEGA32u4 board
- SSD1306
- buttons wired by rows and columns (default 5x2)
- OPTIONAL case such as <a href="https://www.thingiverse.com/thing:4539723">Codek</a>

## Code setup
Set the number of modes you want, title them, and setup your keypad.<br>
Then setup your mode up and mode down buttons. <br>
Last setup your keybinds in `handlePress()`. Each case belongs to your modes in order. <br>

## Usage
Press each key to activate it's keybind for the current mode. To change modes, hold the buttons you defined (by default the far right keys will go up and down respectively)
