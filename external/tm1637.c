#include <stdio.h>
#include <pigpio.h>
#include <stdbool.h>
#include "tm1637.h"

static bool g_inited = false;

//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D
// XGFEDCBA
const unsigned char cDigit2Seg[] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111
};
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


static bool tm1637GetAck(void) {
    bool bAck = false;

    // Clock low, release data line so TM1637 can drive it
    gpioWrite(bClockPin, LOW);
    gpioSetMode(bDataPin, PI_INPUT);
    gpioSetPullUpDown(bDataPin, PI_PUD_UP); // weak pull-up for defined high
    gpioDelay(CLOCK_DELAY);

    // Clock high - TM1637 should pull data low for ACK
    gpioWrite(bClockPin, HIGH);
    gpioDelay(CLOCK_DELAY);

    // Read ACK (0 = success, 1 = NACK)
    bAck = gpioRead(bDataPin) == LOW;

    // Complete ACK cycle
    gpioWrite(bClockPin, LOW);
    gpioDelay(CLOCK_DELAY);

    // Reclaim data line
    gpioSetMode(bDataPin, PI_OUTPUT);
    gpioWrite(bDataPin, LOW);

    return bAck;
}
//
// Write an unsigned char to the controller
//
static bool tm1637WriteByte(unsigned char b) {
    for (int i = 0; i < 8; i++) {
        gpioWrite(bClockPin, LOW);
        gpioWrite(bDataPin, b & 1 ? HIGH : LOW);
        gpioDelay(CLOCK_DELAY);
        gpioWrite(bClockPin, HIGH);
        gpioDelay(CLOCK_DELAY);
        b >>= 1;
    }
    return tm1637GetAck();
} /* tm1637Writeunsigned char() */

//
// Write a sequence of unsigned chars to the controller
//
static bool tm1637Write(const unsigned char *pData, const unsigned char bLen) {
    if (!g_inited) return;
    tm1637Start();
    bool success = true;
    for (unsigned char i = 0; i < bLen; i++) {
        if (!tm1637WriteByte(pData[i])) {
            success = false;
            break;
        }
    }
    tm1637Stop();
    return success;
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
        if (i == 2) {
            if (pString[i] == ':') // turn on correct bit
                bTemp[2] |= 0x80; // second digit high bit controls colon LEDs
        } else {
            unsigned char b = 0;
            if (pString[i] >= '0' && pString[i] <= '9') {
                b = cDigit2Seg[pString[i] & 0xf]; // Extract digit from char
            }
            bTemp[j++] = b;
        }
    }
    tm1637Write(bTemp, j); // send to the display
}
