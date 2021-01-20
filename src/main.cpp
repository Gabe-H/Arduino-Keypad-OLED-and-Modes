#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Keypad.h>
#include <HID-Project.h>
#include <EEPROM.h>

// Uncomment when you want to deploy the keypad. Doesn't seem to be happy when it loses serial connection.
#define PROD true

const byte ROWS = 2; //four rows
const byte COLS = 5; //three columns
const byte NUM_MODES = 5;

char modes[NUM_MODES][20] = { "Discord", "Media", "Numbers", "OBS", "Minecraft" };
char keys[ROWS][COLS] = {
{'5','6','7','8','9'},
{'0','1','2','3','4'}
};


byte rowPins[ROWS] = {16, 10}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {9, 8, 7, 6, 5}; //connect to the column pinouts of the kpd

int mode = 0;
bool pressed = false;
char holdChar;
bool holding = false;
unsigned long holdTimer;

char afkDirs[4] = { KEY_W, KEY_D, KEY_S, KEY_A };
bool afkBool = false;
int afkIndex = 0;
unsigned long afkTimer;

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Adafruit_SSD1306 display(128, 64, &Wire);

String msg;


unsigned long showTime;


void showMode() {
  EEPROM.write(0, mode);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(modes[mode]);
  
  display.println();
  for (int i=0; i<NUM_MODES; i++)
  {
    if (i == mode)
    {
      display.setTextSize(2);
      display.print(i+1);
      display.setTextSize(1);
      display.print(" ");
    }
    else
    {
      display.setTextSize(1);
      display.print(i+1);
      display.print(" ");
    }
  }
  display.display();
}

void handlePress(char key) {
  switch (mode) {
    // Discord
    case 0:
      switch (key) {
        case '0': Keyboard.write(KEY_F13); break;
        case '1': Keyboard.write(KEY_F14); break;
        case '2': Keyboard.write(KEY_F15); break;
        case '3': Keyboard.write(KEY_F16); break;
        case '4': Keyboard.write(KEY_F17); break;
        case '5': Keyboard.write(KEY_F13); break;
        case '6': Keyboard.write(KEY_F14); break;
        case '7': Keyboard.write(KEY_F15); break;
        case '8': Keyboard.write(KEY_F21); break;
        case '9': Keyboard.write(KEY_F22); break;
      }
    break;
    // Media
    case 1:
      switch(key) {
        case '0': Consumer.write(MEDIA_PREVIOUS); break;
        case '1': Consumer.write(MEDIA_PLAY_PAUSE); break;
        case '2': Consumer.write(MEDIA_NEXT); break;
        case '3': Consumer.write(HID_CONSUMER_RANDOM_PLAY);
        case '4': break;
        case '5': Consumer.write(MEDIA_VOL_DOWN); break;
        case '6': Consumer.write(MEDIA_VOL_UP); break;
        case '7': Consumer.write(MEDIA_VOL_MUTE); break;
        case '8': Consumer.write(HID_CONSUMER_REPEAT); break;
        case '9': break;
      }
    break;
    // Numbers
    case 2:
      switch (key) {
        case '0': Keyboard.write(KEY_0); break;
        case '1': Keyboard.write(KEY_1); break;
        case '2': Keyboard.write(KEY_2); break;
        case '3': Keyboard.write(KEY_3); break;
        case '4': Keyboard.write(KEY_4); break;
        case '5': Keyboard.write(KEY_5); break;
        case '6': Keyboard.write(KEY_6); break;
        case '7': Keyboard.write(KEY_7); break;
        case '8': Keyboard.write(KEY_8); break;
        case '9': Keyboard.write(KEY_9); break;
      }
    break;
    // OBS
    case 3:
      Keyboard.press(KEY_RIGHT_ALT);
      switch (key) {
        case '0': Keyboard.press(KEY_F13); break;
        case '1': Keyboard.press(KEY_F14); break;
        case '2': Keyboard.press(KEY_F15); break;
        case '3': Keyboard.press(KEY_F16); break;
        case '4': Keyboard.press(KEY_F17); break;
        case '5': Keyboard.press(KEY_F18); break;
        case '6': Keyboard.press(KEY_F19); break;
        case '7': Keyboard.press(KEY_F20); break;
        case '8': Keyboard.press(KEY_F21); break;
        case '9': Keyboard.press(KEY_F22); break;
      }
    break;
    case 4:
      switch (key) {
        case '0': Keyboard.press(KEY_F5); break;
        case '1': Keyboard.press(KEY_RIGHT_SHIFT); break;
        case '2': Keyboard.press(KEY_ESC); break;
        case '3': Keyboard.press(KEY_G); break;
      }
    break;
  }
  Keyboard.releaseAll();
}

void changeMode(char modeChar) {
  if (modeChar == '9')
  {
    if (mode < NUM_MODES-1)
    {
      mode++;
      showMode();
    }
    else
    {
      mode = 0;
      showMode();
    }
  }
  else if (modeChar == '4')
  {
    if (mode >= 1)
    {
      mode--;
      showMode();
    }
    else
    {
      mode = NUM_MODES-1;
      showMode();
    }
  }
}

void setup() {
  #ifndef PROD
    Serial.begin(9600);
  #endif
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Consumer.begin();
  Keyboard.begin();

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.clearDisplay();
  display.display();
  mode = int(EEPROM.read(0));

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.display();

  msg = "";
  holdTimer = millis();
  afkTimer = millis();
  showMode();
}

void loop() {
  if (holding)
  {
    if (holdChar == '4' || holdChar == '9')
    {
      if ((millis()-holdTimer)>500)
      {
        changeMode(holdChar);
        holdTimer = millis();
      }
    }
    else
    {
      if ((millis()-holdTimer)>31)
      {
        handlePress(holdChar);
        holdTimer = millis();
      }
    }
    
  }
  // Fills kpd.key[ ] array with up-to 10 active keys.
  // Returns true if there are ANY active keys.
  if (kpd.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate)   // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
        {
          case PRESSED:
          msg = " PRESSED.";
          pressed = true;
            // Press single function buttons
          if (kpd.key[i].kchar != '9' && kpd.key[i].kchar != '4')
          {
            handlePress(kpd.key[i].kchar);
          }
        break;
          case HOLD:
          msg = " HOLD.";
            // Set timers and booleans for changing mode vs pressing the button
          pressed = false;
          holding = true;
          holdTimer = millis();
          holdChar = kpd.key[i].kchar;
            // Change if up or down
          changeMode(kpd.key[i].kchar);
        break;
          case RELEASED:
          msg = " RELEASED.";
          holding = false;
          if (pressed)
          {
            if (kpd.key[i].kchar == '9' || kpd.key[i].kchar == '4')
            {
              handlePress(kpd.key[i].kchar);
            }
          }
        break;
          case IDLE:
          msg = " IDLE.";
        }
        #ifndef PROD
        Serial.print("Key ");
        Serial.print(kpd.key[i].kchar);
        Serial.println(msg);
        #endif
      }
    }
  }
}  // End loop