#include <unistd.h>
#include <iostream>

#include "../include/alarm.h"

constexpr int beatDuration = 300000;   // 300ms at 200 BPM (in microseconds)
constexpr int gapDuration = 50000;     // 50ms gap between beats (in microseconds)

bool sound = false;
bool on;

// Makes an alarm-beep-like pattern
void startAlarm() {
    on = true;
    bool previousState = sound;

    while(on) {
        // 3 beats ON
        for (int i = 0; i < 3; i++) {
            sound = true;
            if (sound != previousState) {
                previousState = sound;
            }
            usleep(beatDuration - gapDuration);

            sound = false;
            if (sound != previousState) {
                std::cout << "0" << std::endl;
                previousState = sound;
            }
            usleep(gapDuration);  // Short gap between beats
        }

        // 2 beats OFF
        usleep(2 * beatDuration);
    }
}

void stopAlarm() {
    on = false;
}

bool isRunning() {
    return on;
}
