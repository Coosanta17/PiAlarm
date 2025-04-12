#include <iostream>
#include <pigpio.h>
#include <unistd.h>

#define BUZZER_GPIO 12  // PWM 0
#define BUTTON_GPIO 5
#define A5_FREQUENCY 880  // A5 in Hz
#define PWM_DUTY_CYCLE 500000  // 50% duty cycle

int main() {
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
            gpioHardwarePWM(BUZZER_GPIO, A5_FREQUENCY, PWM_DUTY_CYCLE);
            std::cout << "Playing!" << std::endl;
        } else {
            gpioHardwarePWM(BUZZER_GPIO, 0, 0);
        }

        usleep(100000); // 100ms
    }
    gpioTerminate();
    return 0;
}