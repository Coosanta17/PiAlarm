#include "buzzer.h"

constexpr int beatDuration = 150000; // Microseconds
constexpr int gapDuration = 20000;

std::mutex buzzer_mutex;
bool sound = false;
bool on = false;
unsigned long lastUpdateTime = 0;
int patternPosition = 0;
int beatCount = 0;

void startBuzzer() {
    std::lock_guard lock(buzzer_mutex);
    on = true;
    patternPosition = 0;
    beatCount = 0;
    lastUpdateTime = micros();
}

void stopBuzzer() {
    std::lock_guard lock(buzzer_mutex);
    on = false;
    sound = false;
}

bool isRunning() {
    return on;
}

void updateBuzzer() {
    std::lock_guard lock(buzzer_mutex);
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

            if (beatCount < 4) {
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
