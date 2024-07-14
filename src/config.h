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
#define KNOB_BUTTON_TYPE INPUT_PULLUP
#define KNOB_CLK 15
#define KNOB_DT 14

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define OLED_ADDR 0x3C

// MAX 10 KEYS (product cannot be > 10)
#define ROWS 2 // Rows of buttons
#define COLS 5 // Columns of buttons

byte rowPins[ROWS] = {9, 16};         // Connections for the row pinouts of the kpd
byte colPins[COLS] = {4, 5, 6, 7, 8}; // Connections for the column pinouts of the kpd

/**
 * Keypad button definitions
 * @note CHARACTER DOES NOT MATTER, just unique single characters
 */
char keys[ROWS][COLS] = {
    {'0', '1', '2', '3', '4'},
    {'5', '6', '7', '8', '9'},
};

#define NUM_MODES 6 // Number of modes

/**
 *  Name your modes here!
 * @note Must be 10 chars MAX to fit on the screen!
 */
const char modeNames[NUM_MODES][10] = {"Discord", "Media", "Volume", "F Keys", "Debug", "Plat. IO"};

#define KEY 0x0
#define CON 0x1
#define UNUSED 0x2

#define K_NULL KEY_RESERVED
#define C_NULL HID_CONSUMER_UNASSIGNED

bool modCtrl[NUM_MODES][COLS] = {
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, true, false, false},
    {true, true, true, false, false},
};

bool modShift[NUM_MODES][COLS] = {
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, true, true, true, false},
    {false, false, false, false, false},
};

bool modAlt[NUM_MODES][COLS] = {
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, true},
    {true, true, true, false, false},
};

bool callbackKeys[NUM_MODES][COLS] = {
    {false, false, false, false, true},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
};

/**
 * Keyboard library keybinds
 * MUST USE DEFINITIONS FROM HID-Project - ImprovedKeylayoutsUS.h
 */
KeyboardKeycode keyboardButtons[NUM_MODES][COLS] = {
    {KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17},
    {K_NULL, K_NULL, K_NULL, K_NULL, K_NULL},
    {K_NULL, K_NULL, K_NULL, K_NULL, K_NULL},
    {KEY_F1, KEY_F5, KEY_F10, KEY_F11, KEY_F12},
    {KEY_F5, KEY_F5, KEY_F5, KEY_F12, KEY_F12},
    {KEY_U, KEY_B, KEY_S, K_NULL, K_NULL},
};

/**
 * Consumer (media) library keybinds
 * MUST USE DEFINITIONS FROM ConsumerAPI.h
 */
ConsumerKeycode consumerButtons[NUM_MODES][COLS] = {
    {C_NULL, C_NULL, C_NULL, C_NULL, C_NULL},
    {MEDIA_REWIND, MEDIA_PREVIOUS, MEDIA_PLAY_PAUSE, MEDIA_NEXT, MEDIA_FAST_FORWARD},
    // Volume
    {
        HID_CONSUMER_VOLUME_DECREMENT, HID_CONSUMER_VOLUME_INCREMENT, HID_CONSUMER_MUTE,
        HID_CONSUMER_VOLUME_DECREMENT, HID_CONSUMER_VOLUME_INCREMENT},
    // F Keys (Keyboard)
    {C_NULL, C_NULL, C_NULL, C_NULL, C_NULL},
    // Debug
    {C_NULL, C_NULL, C_NULL, C_NULL, C_NULL},
    // Platform IO
    {C_NULL, C_NULL, C_NULL, C_NULL, C_NULL},
};