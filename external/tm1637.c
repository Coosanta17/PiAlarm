#include <stdio.h>
#include <pigpio.h>
#include <stdbool.h>
#include "tm1637.h"

static bool g_inited = false;
const unsigned char cDigit2Seg[] = {0x3f, 0x6, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
static unsigned char bClockPin, bDataPin;
#define CLOCK_DELAY 5
#define LOW  0
#define HIGH 1

//
// Initialize the tm1637 by specifying the digital pins used to communicate
// returns 0 for success, -1 for failure
//
int tm1637Init(const unsigned char bClock, const unsigned char bData) {
    if (gpioInitialise() < 0) {
        printf("Error initializing pigpio library\n");
        return -1;
    }
    bClockPin = bClock;
    bDataPin = bData;
    gpioSetMode(bClockPin, PI_OUTPUT);
    gpioSetMode(bDataPin, PI_OUTPUT);
    gpioWrite(bClockPin, LOW);
    gpioWrite(bDataPin, LOW);
    g_inited = true;
    return 0;
} /* tm1637Init() */

//
// Start wire transaction
//
static void tm1637Start(void) {
    gpioWrite(bDataPin, HIGH);
    gpioWrite(bClockPin, HIGH);
    gpioDelay(CLOCK_DELAY);
    gpioWrite(bDataPin, LOW);
} /* tm1637Start() */

//
// Stop wire transaction
//
static void tm1637Stop(void) {
    gpioWrite(bClockPin, LOW);
    gpioDelay(CLOCK_DELAY);
    gpioWrite(bDataPin, LOW);
    gpioDelay(CLOCK_DELAY);
    gpioWrite(bClockPin, HIGH);
    gpioDelay(CLOCK_DELAY);
    gpioWrite(bDataPin, HIGH);
} /* tm1637Stop() */

//
// Write an unsigned char to the controller
//
static void tm1637WriteByte(unsigned char b) {
    for (int i = 0; i < 8; i++) {
        gpioWrite(bClockPin, LOW);
        gpioWrite(bDataPin, b & 1 ? HIGH : LOW);
        gpioDelay(CLOCK_DELAY);
        gpioWrite(bClockPin, HIGH);
        gpioDelay(CLOCK_DELAY);
        b >>= 1;
    }
} /* tm1637Writeunsigned char() */

//
// Write a sequence of unsigned chars to the controller
//
static void tm1637Write(const unsigned char *pData, const unsigned char bLen) {
    if (!g_inited) return;
    tm1637Start();
    for (unsigned char i = 0; i < bLen; i++) {
        tm1637WriteByte(pData[i]);
    }
    tm1637Stop();
} /* tm1637Write() */

//
// Set brightness (0-8)
//
void tm1637SetBrightness(unsigned char b) {
    if (!g_inited) return;
    unsigned char bControl;
    if (b == 0) // display off
        bControl = 0x80; // display off
    else {
        if (b > 8) b = 8;
        bControl = 0x88 | b - 1;
    }
    tm1637Write(&bControl, 1);
} /* tm1637SetBrightness() */

//
// Display a string of 4 digits and optional colon
// by passing a string such as "12:34" or "45 67"
//
void tm1637ShowDigits(const char *pString) {
    if (!g_inited) return;
    unsigned char bTemp[16]; // commands and data to transmit

    unsigned char j = 0;
    bTemp[0] = 0x40; // memory write command (auto increment mode)
    tm1637Write(bTemp, 1);

    bTemp[j++] = 0xc0; // set display address to first digit command
    for (unsigned char i = 0; i < 5; i++) {
        if (i == 2) // position of the colon
        {
            if (pString[i] == ':') // turn on correct bit
                bTemp[2] |= 0x80; // second digit high bit controls colon LEDs
        } else {
            unsigned char b = 0;
            if (pString[i] >= '0' && pString[i] <= '9') {
                b = cDigit2Seg[pString[i] & 0xf]; // segment data
            }
            bTemp[j++] = b;
        }
    }
    tm1637Write(bTemp, j); // send to the display
}
