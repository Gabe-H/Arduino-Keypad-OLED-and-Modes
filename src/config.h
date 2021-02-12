#include <Arduino.h>

/**
 *  Timeout until screensaver (ms)
 */

#define SCREENSAVER_TIMEOUT 2500

/**
 *  Uncomment to use rotary encoder for volume while idling.
 */

// #define ENCODER_AS_VOLUME

/**
 * Uncomment to log Serial
 * @note Leaving this on may lead to performance issues if not being monitored.
 */

// #define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
  #define SERIAL_BAUD 9600
#endif

/**
 * Uncomment to clear/reset EEPROM
 * @note Leaving this on won't save modes between power cycles!
 */

// #define EEPROM_INIT

#define KNOB_BUTTON A0
#define KNOB_CLK 15
#define KNOB_DT 14

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define OLED_ADDR 0x3C

// MAX 10 KEYS (product cannot be > 10)
#define ROWS 2 // Rows of buttons
#define COLS 5 // Columns of buttons

byte rowPins[ROWS] = {9, 16}; // Connections for the row pinouts of the kpd
byte colPins[COLS] = {4, 5, 6, 7, 8}; // Connections for the column pinouts of the kpd

/**
 * Keypad button definitions
 * @note CHARACTER DOES NOT MATTER, just unique single characters
 */
char keys[ROWS][COLS] = {
{'0','1','2','3','4'},
{'5','6','7','8','9'}
};

#define NUM_MODES 4 // Number of modes

/**
 *  Name your modes here!
 * @note Must be 10 chars MAX to fit on the screen!
 */
const char modeNames[NUM_MODES][10] = { "Discord", "Media", "Volume", "F Keys"};


#define K 0
#define C 1
#define UNUSED 2
/** Table of key types (Keyboard vs Consumer)
 * K for Keybaord
 * C for Consumer
 */
uint8_t buttonType[NUM_MODES][COLS] = {
  {
    K, K, K, K, K
  },
  {
    C, C, C, C, C
  },
  {
    C, C, C, C, C
  },
  {
    K, K, K, K, K
  }
};

/**
 * Keyboard library keybinds
 * @note MUST USE DEFINITIONS FROM HID-Project - ImprovedKeylayoutsUS.h
 */
KeyboardKeycode keyboardButtons[NUM_MODES][COLS] = {
  // Discord
  {
    KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17
  },
  // Media (consumer)
  {},
  // Volume (consumer)
  {},
  // F Keys
  {
    KEY_F1, KEY_F5, KEY_F10, KEY_F11, KEY_F12
  }
};

/**
 * Consumer (media) library keybinds
 * @note MUST USE DEFINITIONS FROM ConsumerAPI.h
 */
ConsumerKeycode consumerButtons[NUM_MODES][COLS] = {
  // Discord (Keyboard)
  {},
  // Media
  {
    MEDIA_REWIND, MEDIA_PREVIOUS, MEDIA_PLAY_PAUSE, MEDIA_NEXT, MEDIA_FAST_FORWARD
  },
  // Volume
  {
    HID_CONSUMER_VOLUME_DECREMENT, HID_CONSUMER_VOLUME_INCREMENT, HID_CONSUMER_MUTE, 
    HID_CONSUMER_VOLUME_DECREMENT, HID_CONSUMER_VOLUME_INCREMENT
  },
  // F Keys (Keyboard)
  {}
};