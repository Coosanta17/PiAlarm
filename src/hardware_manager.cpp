#include "hardware_manager.h"
#include <pigpio.h>
#include <tm1637.h>
#include <stdexcept>
#include <csignal>

namespace {
    void initializeGpio() {
        if (gpioInitialise() < 0) {
            throw std::runtime_error("PIGPIO initialization failed");
        }

        gpioSetMode(HardwareManager::BUZZER_GPIO, PI_OUTPUT);
        gpioSetMode(HardwareManager::BUTTON_GPIO, PI_INPUT);
        gpioSetPullUpDown(HardwareManager::BUTTON_GPIO, PI_PUD_UP);
    }

    void initializeDisplay() {
        if (tm1637Init(HardwareManager::DISPLAY_CLOCK_GPIO, HardwareManager::DISPLAY_DATA_GPIO) == -1) {
            throw std::runtime_error("TM1637 initialization failed");
        }
    }
}

namespace HardwareManager {
    void initialize() {
        initializeGpio();
        initializeDisplay();
    }

    void setBuzzerState(const bool enabled) {
        if (enabled) {
            gpioHardwarePWM(BUZZER_GPIO, FREQUENCY, PWM_DUTY_CYCLE);
        } else {
            gpioHardwarePWM(BUZZER_GPIO, 0, 0);
        }
    }

    void cleanup() {
        gpioHardwarePWM(BUZZER_GPIO, 0, 0);
        gpioTerminate();
    }
}
