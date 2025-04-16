#include <iostream>
#include <pigpio.h>
#include "../external/pigpio.h" // For development purposes only to remove red text
#include <unistd.h>
#include <csignal>
#include "../include/alarm.h"

#define BUZZER_GPIO 12  // PWM 0
#define BUTTON_GPIO 5
#define A5_FREQUENCY 880  // A5 in Hz
#define PWM_DUTY_CYCLE 300000  // 30% duty cycle

bool &sound_state = sound;
int lastButtonState = 1;
volatile bool running = true;

void signalHandler(const int signum) {
    std::cout << "\nInterrupt signal received. (" << signum << ")" << std::endl;
    running = false;
}

int main() {
    gpioSetSignalFunc(SIGINT, signalHandler);

    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed" << std::endl;
        return 1;
    }

    gpioSetMode(BUZZER_GPIO, PI_OUTPUT);

    gpioSetMode(BUTTON_GPIO, PI_INPUT);
    gpioSetPullUpDown(BUTTON_GPIO, PI_PUD_UP);

    std::cout << "Press button to toggle alarm!" << std::endl;

    while (running) {
        const int buttonState = gpioRead(BUTTON_GPIO);

        if (buttonState == 0 && lastButtonState == 1) {
            if (!isRunning()) {
                startAlarm();
            } else {
                stopAlarm();
            }
        }
        lastButtonState = buttonState;

        updateAlarm();

        if (sound_state) {
            gpioHardwarePWM(BUZZER_GPIO, A5_FREQUENCY, PWM_DUTY_CYCLE);
        } else {
            gpioHardwarePWM(BUZZER_GPIO, 0, 0);
        }

        usleep(10000); // 10ms
    }

    gpioHardwarePWM(BUZZER_GPIO, 0, 0);
    if (isRunning()) {
        stopAlarm();
    }

    gpioTerminate();
    std::cout << "Exiting." << std::endl;
    return 0;
}
