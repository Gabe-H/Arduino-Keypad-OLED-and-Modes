#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Keypad.h>
#include <Encoder.h>
#include <HID-Project.h>
#include <EEPROM.h>

/***********************************************
 * Arduino Keypad with OLED and Modes
 * by Gabe Haarberg
 * 
 * For customization, change the keypad and mode settings on the next few lines,
 * and specific commands further down in `determineKey()`. Refer to HID-Project
 * docs for keybind syntax.
 * 
 * @note ON THE FIRST UPLOAD uncomment the EEPROM clearing. 
 * 
 **********************************************/

#define KNOB_BUTTON A0
#define KNOB_CLK 15
#define KNOB_DT 14

#define ROWS 2 // Rows of buttons
#define COLS 5 // Columns of buttons
#define SCREENSAVER_TIMEOUT 2500 // Timeout until screensaver (ms)
#define ENCODER_AS_VOLUME true // Will use encoder for volume when on idle, press to enter editing

#define NUM_MODES 6 // Number of modes. MORE THAN 10 NOT RECOMMENDED

/* Name your modes here! */
char modes[NUM_MODES][20] = { "Discord", "Media", "Volume", "Numbers", "Lower F", "Upper F" };

// Keypad button definitions
char keys[ROWS][COLS] = {
{'5','6','7','8','9'},
{'0','1','2','3','4'}
};

byte rowPins[ROWS] = {16, 10}; // Connections for the row pinouts of the kpd
byte colPins[COLS] = {9, 8, 7, 6, 5}; // Connections for the column pinouts of the kpd

int activeModes[ROWS] = { 0, 0 }; // Store the active mode for each row
uint8_t activeRow = 0;      // Row that is/was being edited

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Adafruit_SSD1306 display(128, 64, &Wire);
Encoder enc(KNOB_CLK, KNOB_DT);

String msg; // Placeholder for Serial output per action
int oldButtonState; // Global placeholders
long oldKnobPosition;
bool editMode;
unsigned long displayTimeout;

// Makes button actions with the Keyboard library
void keyPress(KeyboardKeycode key, bool press) {
  if (press) Keyboard.press(key);
  else Keyboard.release(key);
}

// Makes button actions with the Consumer library
void consumerPress(ConsumerKeycode key, bool press) {
  if (press) Consumer.press(key);
  else Consumer.release(key);
}

/**************************************
 * Set your macros here!
 * 
 * @note Depending on the key use keyPress() or consumerPress().
 *  For the most part, keys typically on a keyboard will
 *  use keyPress(), and extra keys (media) will use
 *  consumerPress().
 * @note Only make integer cases for each *column* in your keypad,
 *  not every key on the pad.
 * 
 * You may always refer to the HID-Project docs for further
 * clarification
 *************************************/

void determineKey(char key, bool press) {
  int keyRow;
  int keyCol;
  // Get the key's column and row
  for (int r=0; r<ROWS; r++)
  {
    for (int c=0; c<COLS; c++)
    {
      if (key == keys[r][c])
      {
        keyRow = r;
        keyCol = c;
      }
    }
  }
  // Activate based on the row's mode
  switch(activeModes[keyRow])
  {
    // Discord
    case 0:
      switch (keyCol)
      {
        case 0: keyPress(KEY_F13, press); break;
        case 1: keyPress(KEY_F14, press); break;
        case 2: keyPress(KEY_F15, press); break;
        case 3: keyPress(KEY_F16, press); break;
        case 4: keyPress(KEY_F17, press); break;
      }
      break;
    // Media
    case 1:
      switch (keyCol)
      {
        case 0: consumerPress(MEDIA_REWIND, press); break;
        case 1: consumerPress(MEDIA_PREVIOUS, press); break;
        case 2: consumerPress(MEDIA_PLAY_PAUSE, press); break;
        case 3: consumerPress(MEDIA_NEXT, press); break;
        case 4: consumerPress(MEDIA_FAST_FORWARD, press); break;
      }
      break;
    // Volume
    case 2:
      switch (keyCol)
      {
        case 0: consumerPress(HID_CONSUMER_VOLUME_DECREMENT, press); break;
        case 1: consumerPress(HID_CONSUMER_VOLUME_INCREMENT, press); break;
        case 2: consumerPress(HID_CONSUMER_MUTE, press); break;
        case 3: consumerPress(HID_CONSUMER_VOLUME_DECREMENT, press); break;
        case 4: consumerPress(HID_CONSUMER_VOLUME_INCREMENT, press); break;
      }
      break;
    // Numbers
    case 3:
      switch (keyCol)
      {
        case 0: keyPress(KEY_1, press); break;
        case 1: keyPress(KEY_2, press); break;
        case 2: keyPress(KEY_3, press); break;
        case 3: keyPress(KEY_4, press); break;
        case 4: keyPress(KEY_5, press); break;
      }
      break;
    // Lower F
    case 4:
      switch (keyCol)
      {
        case 0: keyPress(KEY_F1, press); break;
        case 1: keyPress(KEY_F2, press); break;
        case 2: keyPress(KEY_F3, press); break;
        case 3: keyPress(KEY_F4, press); break;
        case 4: keyPress(KEY_F5, press); break;
      }
      break;
    // Upper F
    case 5:
      switch (keyCol)
      {
        case 0: keyPress(KEY_F8, press); break;
        case 1: keyPress(KEY_F9, press); break;
        case 2: keyPress(KEY_F10, press); break;
        case 3: keyPress(KEY_F11, press); break;
        case 4: keyPress(KEY_F12, press); break;
      }
      break;
  }
}

// Index 0 is for the editing row, all following bytes are
//  used for the active mode of each row
void saveEEPROM() {
  EEPROM.write(0, activeRow);
  
  for (int i=0; i<ROWS; i++)
  {
    EEPROM.write(i+1, activeModes[i]);
  }
}

// Shows the editing data on the screen.
// Uses '-' and 'O' like a slider:
// O----, --O--, ---O-
void refreshDisplay() {
  // Reset all writing parameters
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(modes[activeModes[activeRow]]);

  // The active editing row will be larger than the other row(s)
  for (int d=0; d<ROWS; d++)
  {
    if (d == activeRow) display.setTextSize(2);
    else display.setTextSize(1);

    for (int i=0; i<NUM_MODES; i++)
    {
      if (i == activeModes[d])
      {
        display.print(F("O"));
      }
      else
      {
        display.print(F("-"));
      }
    }
    display.println();
  }

  editMode = true; // Start screensaver timer
  displayTimeout = millis();
  saveEEPROM(); // Save new settings to EEPROM
  display.display(); // Update the display
}

// On key state change, iterate and perform press or release
//  for the active key
void handleKeys() {
  if (kpd.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED:
            msg = " PRESSED.";
            determineKey(kpd.key[i].kchar, true);
          break;
          case RELEASED:
            msg = " RELEASED.";
            determineKey(kpd.key[i].kchar, false);
          break;
        }

        Serial.print("Key ");
        Serial.print(kpd.key[i].kchar);
        Serial.println(msg);
      }
    }
  }
}

// Function dedicated to rotary encoder logic
void handleKnob() {
  // Setup new reading variables
  long newKnobPosition = enc.read();
  int newButtonState = digitalRead(KNOB_BUTTON);

  if (newKnobPosition - oldKnobPosition == 4) // Was turned 1 click clockwise
  {
    if (editMode)
    {
      Serial.println("UP");
      activeModes[activeRow]++;
      if (activeModes[activeRow] > NUM_MODES-1) activeModes[activeRow] = 0;
      refreshDisplay();
    }
    else if (ENCODER_AS_VOLUME)
    {
      Consumer.write(HID_CONSUMER_VOLUME_INCREMENT);
    }
    else
    {
      refreshDisplay();
    }
    
    oldKnobPosition = newKnobPosition;
  }
  if (newKnobPosition - oldKnobPosition == -4) // Was turned 1 click counter-clockwise
  {
    if (editMode)
    {
      Serial.println("DOWN");
      activeModes[activeRow]--;
      if (activeModes[activeRow] < 0) activeModes[activeRow] = NUM_MODES-1;
      refreshDisplay();
    }
    else if (ENCODER_AS_VOLUME)
    {
      Consumer.write(HID_CONSUMER_VOLUME_DECREMENT);
    }
    else
    {
      refreshDisplay();
    }
    
    oldKnobPosition = newKnobPosition;
  }
  // Button logic
  if (newButtonState != oldButtonState)
  {
    if (newButtonState == LOW)
    {
      if (editMode)
      {
        Serial.println("Button pressed");
        activeRow++;
        if (activeRow > ROWS-1) activeRow = 0;
      }

      refreshDisplay();
    }
  }
  oldButtonState = newButtonState;
}

// Shows only the active modes for each row, top to bottom.
// By standard will only have capacity for 3 rows, but delete the
// first display.println(), and change display.setTextSize(1); for more capacity
void screenSaver() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println();
  for (int i=0; i<ROWS; i++)
  {
    display.print(" ");
    display.println(modes[activeModes[i]]);
  }
  editMode = false;
  display.display();
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  EEPROM.begin();

  // // UNCOMMENT THE NEXT LINES ON THE FIRST UPLOAD
  //
  // for (int i=0; i<16; i++)
  // {
  //   EEPROM.write(i, 0x00);
  // }
  //
  // // END FIRST UPLOAD UNCOMMENT

  pinMode(KNOB_BUTTON, INPUT_PULLUP);
  oldKnobPosition = enc.read();
  activeRow = EEPROM.read(0);

  // Get saved modes per row
  for (int i=0; i<ROWS; i++)
  {
    uint8_t read = EEPROM.read(i+1);
    if (read != 0) activeModes[i] = read;
    else activeModes[i] = 0;
  }

  // Initiate SSD1306 OLED
  display.clearDisplay();
  display.display();

  display.setTextColor(SSD1306_WHITE);
  screenSaver();

  displayTimeout = millis(); // Start screensaver timer
  Keyboard.begin();
  Consumer.begin();
  Serial.begin(9600);
}

void loop() {
  handleKeys();
  handleKnob();
  
  // If the display is being edited and it has been more than the specified ms
  if (editMode && (millis() - displayTimeout) > SCREENSAVER_TIMEOUT)
  {
    Serial.println("Screen saver");
    screenSaver();
  }
}
