#include <iostream>
#include "../external/pigpio.h"
#include <unistd.h>
#include "../include/alarm.h"

#define BUZZER_GPIO 12  // PWM 0
#define BUTTON_GPIO 5
#define A5_FREQUENCY 880  // A5 in Hz
#define PWM_DUTY_CYCLE 500000  // 50% duty cycle

bool &sound_state = sound;

[[noreturn]] int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed" << std::endl;
        return 1;
    }

    gpioSetMode(BUZZER_GPIO, PI_OUTPUT);

    gpioSetMode(BUTTON_GPIO, PI_INPUT);
    gpioSetPullUpDown(BUTTON_GPIO, PI_PUD_UP);

    std::cout << "Press button to buzz!" << std::endl;

    while (true) {
        if (const int buttonState = gpioRead(BUTTON_GPIO); buttonState == 0) {
            if (!isRunning()) {
                startAlarm();
            } else if (sound_state) {
                gpioHardwarePWM(BUZZER_GPIO, A5_FREQUENCY, PWM_DUTY_CYCLE);
            }

        } else {
            stopAlarm();
            gpioHardwarePWM(BUZZER_GPIO, 0, 0);
        }

        usleep(10000); // 10ms
    }
    gpioTerminate();
    return 0;
}