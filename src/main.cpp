#include <iostream>
#include <pigpio.h>
#include <unistd.h>

#define BUZZER_GPIO 5
#define BUTTON_GPIO 18

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed" << std::endl;
        return 1;
    }

    // Setup
    gpioSetMode(BUZZER_GPIO, PI_OUTPUT);
    gpioSetMode(BUTTON_GPIO, PI_INPUT);
    gpioSetPullUpDown(BUTTON_GPIO, PI_PUD_UP);

    std::cout << "Press button to buzz!" << std::endl;

    while (true) {
        int buttonState = gpioRead(BUTTON_GPIO);

        if (buttonState == 0) {
            gpioWrite(BUZZER_GPIO, 1);
            std::cout << "Buzzing!" << std::endl;
        } else {
            gpioWrite(BUZZER_GPIO, 0);
        }

        usleep(100000); // 100ms
    }

    gpioTerminate();
    return 0;
}
