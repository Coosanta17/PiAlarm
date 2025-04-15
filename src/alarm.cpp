#include <unistd.h>
#include <iostream>
#include <ctime>
#include "../include/alarm.h"

constexpr int beatDuration = 300000;   // 300ms at 200 BPM (in microseconds)
constexpr int gapDuration = 50000;     // 50ms gap between beats (in microseconds)

bool sound = false;
bool on = false;
unsigned long lastUpdateTime = 0;
int patternPosition = 0;
int beatCount = 0;

void startAlarm() {
    on = true;
    patternPosition = 0;
    beatCount = 0;
    lastUpdateTime = micros();
}

void stopAlarm() {
    on = false;
    sound = false;
}

bool isRunning() {
    return on;
}

void updateAlarm() {
    if (!on) return;

    const unsigned long currentTime = micros();
    const unsigned long elapsed = currentTime - lastUpdateTime;

    if (patternPosition == 0) {
        if (!sound) {
            sound = true;
        }

        if (elapsed >= beatDuration - gapDuration) {
            patternPosition = 1;
            lastUpdateTime = currentTime;
        }
    } else if (patternPosition == 1) {
        if (sound) {
            sound = false;
        }

        if (elapsed >= gapDuration) {
            beatCount++;

            if (beatCount < 3) {
                patternPosition = 0;
            } else {
                patternPosition = 2;
                beatCount = 0;
            }

            lastUpdateTime = currentTime;
        }
    } else if (patternPosition == 2) {
        if (sound) {
            sound = false;
        }

        if (elapsed >= 2 * beatDuration) {
            patternPosition = 0;
            lastUpdateTime = currentTime;
        }
    }
}

unsigned long micros() {
    timespec ts = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}