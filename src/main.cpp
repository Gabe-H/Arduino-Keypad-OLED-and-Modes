#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Keypad.h>
#include <Encoder.h>
#include <HID-Project.h>
#include <EEPROM.h>

#include "config.h"

/***********************************************
 * Arduino Keypad with OLED and Modes
 * by Gabe Haarberg
 * 
 * https://github.com/Gabe-H/Arduino-Keypad-OLED-and-Modes
 * 
 * For customization, change the keypad and mode settings on the next few lines,
 * and specific commands further down in `determineKey()`. Refer to HID-Project
 * docs for keybind syntax.
 * 
 * @note ON THE FIRST UPLOAD uncomment the EEPROM clearing. 
 * 
 **********************************************/


int activeModes[ROWS]; // Store the active mode for each row
int activeRow = 0;      // Row that is/was being edited

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);
Encoder enc(KNOB_CLK, KNOB_DT);

String msg; // Placeholder for Serial output per action
int oldButtonState; // Global placeholders
long oldKnobPosition;
bool editMode;
unsigned long displayTimeout;


// Makes button actions with the Keyboard library
void keyPress(KeyboardKeycode key, bool press) {
  press ? Keyboard.press(key) : Keyboard.release(key);
}

// Makes button actions with the Consumer library
void consumerPress(ConsumerKeycode key, bool press) {
  press ? Consumer.press(key) : Consumer.release(key);
}

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
  #ifdef DEBUG_SERIAL
    if (press)
    {
      Serial.print(keyCol);
      Serial.print(", ");
      Serial.println(keyRow);
    }
  #endif

  // Determine what function to use on keybind from the buttonType table
  int myType = buttonType[ activeModes[keyRow] ][ keyCol ];
  switch (myType)
  {
    // Keyboard library keys
    case KEY:
    {
      KeyboardKeycode myKey = keyboardButtons[ activeModes[keyRow] ][ keyCol ];
      #ifdef DEBUG_SERIAL
        Serial.print("Keyboard: ");
        Serial.println(myKey);
        #endif
      press ? Keyboard.press(myKey) : Keyboard.release(myKey);
    }
    break;
    // Consumer library keys
    case CON:
    {
      ConsumerKeycode myKey = consumerButtons[ activeModes[keyRow] ][ keyCol ];
      #ifdef DEBUG_SERIAL
        Serial.print("Consumer: ");
        Serial.println(myKey);
        #endif
      press ? Consumer.press(myKey) : Consumer.release(myKey);
    }
    break;
    // Marked as unused
    case UNUSED:
    {
      #ifdef DEBUG_SERIAL
        Serial.println("Unused");
        #endif
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
  display.println(modeNames[activeModes[activeRow]]);

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
          {
            msg = " PRESSED.";
            determineKey(kpd.key[i].kchar, true);
          }
          break;
          case RELEASED:
          {
            msg = " RELEASED.";
            determineKey(kpd.key[i].kchar, false);
          }
          break;
          case HOLD:
          {
            msg = " HOLD.";
          }
          break;
          case IDLE:
          {
            msg = " IDLE.";
          }
          break;
        }

        #ifdef DEBUG_SERIAL
          Serial.print("Key ");
          Serial.print(kpd.key[i].kchar);
          Serial.println(msg);
          #endif
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
      activeModes[activeRow]++;
      if (activeModes[activeRow] > NUM_MODES-1) activeModes[activeRow] = 0;
      refreshDisplay();
    }
    else
    {
      #ifdef ENCODER_AS_VOLUME
        Consumer.write(HID_CONSUMER_VOLUME_INCREMENT);
        #else
        refreshDisplay();
        #endif
    }
    
    oldKnobPosition = newKnobPosition;
  }
  if (newKnobPosition - oldKnobPosition == -4) // Was turned 1 click counter-clockwise
  {
    if (editMode)
    {
      activeModes[activeRow]--;
      if (activeModes[activeRow] < 0) activeModes[activeRow] = NUM_MODES-1;
      refreshDisplay();
    }
    else
    {
      #ifdef ENCODER_AS_VOLUME
        Consumer.write(HID_CONSUMER_VOLUME_DECREMENT);
        #else
        refreshDisplay();
        #endif
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
    display.println(modeNames[activeModes[i]]);
  }
  editMode = false;
  display.display();
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  EEPROM.begin();

  #ifdef EEPROM_INIT
    for (int i=0; i<16; i++)
    {
      EEPROM.write(i, 0x00);
    }
    #endif

  pinMode(KNOB_BUTTON, KNOB_BUTTON_TYPE);
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
  #ifdef DEBUG_SERIAL
    Serial.begin(SERIAL_BAUD);
    #endif
}

void loop() {
  handleKeys();
  handleKnob();
  
  // If the display is being edited and it has been more than the specified ms
  if (editMode && (millis() - displayTimeout) > SCREENSAVER_TIMEOUT)
  {
    screenSaver();
  }
}
