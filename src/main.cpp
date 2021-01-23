#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Keypad.h>
#include <Encoder.h>
#include <HID-Project.h>
#include <EEPROM.h>

#define KNOB_BUTTON A0

#define ROWS 2 // two rows
#define COLS 5 // five columns

#define NUM_MODES 6

char modes[NUM_MODES][20] = { "Discord", "Media", "Volume", "Numbers", "Lower F", "Upper F" };

char keys[ROWS][COLS] = {
{'5','6','7','8','9'},
{'0','1','2','3','4'}
};

byte rowPins[ROWS] = {16, 10}; // connect to the row pinouts of the kpd
byte colPins[COLS] = {9, 8, 7, 6, 5}; // connect to the column pinouts of the kpd

uint8_t activeModes[NUM_MODES] = { 0, 0, 0, 0, 0, 0 };
uint8_t activeRow = 0;

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Adafruit_SSD1306 display(128, 64, &Wire);
Encoder enc(15, 14);

String msg;
int oldButtonState;
long oldKnobPosition;
bool editMode;
unsigned long displayTimeout;

void keyPress(KeyboardKeycode key, bool press) {
  if (press)
  {
    Keyboard.press(key);
  }
  else
  {
    Keyboard.release(key);
  }
}

void consumerPress(ConsumerKeycode key, bool press) {
  if (press)
  {
    Consumer.press(key);
  }
  else
  {
    Consumer.release(key);
  }
}

void determineKey(char key, bool press) {
  uint8_t rowMode = 0;
  // Get what row the key is in
  for (int r=0; r<ROWS; r++)
  {
    for (int c=0; c<COLS; c++)
    {
      if (key == keys[r][c])
      {
        rowMode = r;
      }
    }
  }
  // Activate based on the row's mode
  switch(activeModes[rowMode])
  {
    // Discord
    case 0:
      switch (key)
      {
        case '0': case '5': keyPress(KEY_F13, press);
          break;
        case '1': case '6': keyPress(KEY_F14, press);
          break;
        case '2': case '7': keyPress(KEY_F15, press);
          break;
        case '3': case '8': keyPress(KEY_F16, press);
          break;
        case '4': case '9': keyPress(KEY_F17, press);
          break;
      }
      break;
    // Media
    case 1:
      switch (key)
      {
        case '0': case '5': consumerPress(MEDIA_REWIND, press);
          break;
        case '1': case '6': consumerPress(MEDIA_PREVIOUS, press);
          break;
        case '2': case '7': consumerPress(MEDIA_PLAY_PAUSE, press);
          break;
        case '3': case '8': consumerPress(MEDIA_NEXT, press);
          break;
        case '4': case '9': consumerPress(MEDIA_FAST_FORWARD, press);
          break;
      }
      break;
    // Volume
    case 2:
      switch (key)
      {
        case '0': case '5': consumerPress(HID_CONSUMER_VOLUME_DECREMENT, press);
          break;
        case '1': case '6': consumerPress(HID_CONSUMER_VOLUME_INCREMENT, press);
          break;
        case '2': case '7': consumerPress(HID_CONSUMER_MUTE, press);
          break;
        case '3': case '8': consumerPress(HID_CONSUMER_VOLUME_DECREMENT, press);
          break;
        case '4': case '9': consumerPress(HID_CONSUMER_VOLUME_INCREMENT, press);
          break;
      }
      break;
    // Numbers
    case 3:
      switch (key)
      {
        case '0': case '5': keyPress(KEY_1, press);
          break;
        case '1': case '6': keyPress(KEY_2, press);
          break;
        case '2': case '7': keyPress(KEY_3, press);
          break;
        case '3': case '8': keyPress(KEY_4, press);
          break;
        case '4': case '9': keyPress(KEY_5, press);
          break;
      }
      break;
    // Lower F
    case 4:
      switch (key)
      {
        case '0': case '5': keyPress(KEY_F1, press);
          break;
        case '1': case '6': keyPress(KEY_F2, press);
          break;
        case '2': case '7': keyPress(KEY_F3, press);
          break;
        case '3': case '8': keyPress(KEY_F4, press);
          break;
        case '4': case '9': keyPress(KEY_F5, press);
          break;
      }
      break;
    // Upper F
    case 5:
      switch (key)
      {
        case '0': case '5': keyPress(KEY_F8, press);
          break;
        case '1': case '6': keyPress(KEY_F9, press);
          break;
        case '2': case '7': keyPress(KEY_F10, press);
          break;
        case '3': case '8': keyPress(KEY_F11, press);
          break;
        case '4': case '9': keyPress(KEY_F12, press);
          break;
      }
      break;
  }
}

void refreshDisplay() {
  editMode = true;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(modes[activeModes[activeRow]]);

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
  displayTimeout = millis();
  display.display();
}

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
          case HOLD:
            msg = " HOLD.";
          break;
          case RELEASED:
            msg = " RELEASED.";
            determineKey(kpd.key[i].kchar, false);
          break;
          case IDLE:
            msg = " IDLE.";
          break;
        }

        Serial.print("Key ");
        Serial.print(kpd.key[i].kchar);
        Serial.println(msg);
      }
    }
  }
}

void saveEEPROM() {
  EEPROM.write(0, activeRow);
  
  for (int i=0; i<ROWS; i++)
  {
    EEPROM.write(i+1, activeModes[i]);
  }
}

void handleKnob() {
  long newKnobPosition = enc.read();
  int newButtonState = digitalRead(KNOB_BUTTON);

  if (newKnobPosition - oldKnobPosition == 4)
  {
    if (editMode)
    {
      Serial.println("UP");
      activeModes[activeRow]++;
      if (activeModes[activeRow] > NUM_MODES-1) activeModes[activeRow] = 0;
    }
    
    refreshDisplay();
    
    oldKnobPosition = newKnobPosition;
  }
  if (newKnobPosition - oldKnobPosition == -4)
  {
    if (editMode)
    {
      Serial.println("DOWN");
      activeModes[activeRow]--;
      if (activeModes[activeRow] == 255) activeModes[activeRow] = NUM_MODES-1;
    }

    refreshDisplay();
    
    oldKnobPosition = newKnobPosition;
  }
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
  display.display();
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  EEPROM.begin();

  pinMode(KNOB_BUTTON, INPUT_PULLUP);
  oldKnobPosition = enc.read();
  // for (int i=0; i<16; i++)
  // {
  //   EEPROM.write(i, 0x00);
  // }
  activeRow = EEPROM.read(0);

  for (int i=0; i<ROWS; i++)
  {
    uint8_t read = EEPROM.read(i+1);
    if (read != 0) activeModes[i] = read;
    else activeModes[i] = 0;
  }

  display.clearDisplay();
  display.display();
  delay(500);
  display.clearDisplay();
  display.display();

  display.setTextColor(SSD1306_WHITE);
  screenSaver();

  displayTimeout = millis();
  Keyboard.begin();
  Consumer.begin();
  Serial.begin(9600);
}

void loop() {
  handleKeys();
  handleKnob();
  
  if (editMode)
  {
    if ((millis() - displayTimeout) > 2500)
    {
      Serial.println("Screen saver");
      saveEEPROM();
      screenSaver();
      editMode = false;
    }
  }
}
