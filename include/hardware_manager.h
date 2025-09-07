#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

namespace HardwareManager {
    inline constexpr int BUZZER_GPIO = 12;
    inline constexpr int BUTTON_GPIO = 5;
    inline constexpr int DISPLAY_CLOCK_GPIO = 23;
    inline constexpr int DISPLAY_DATA_GPIO = 24;

    inline constexpr int FREQUENCY = 880;
    inline constexpr int PWM_DUTY_CYCLE = 500000;

    void initialize();
    void cleanup();
    void setBuzzerState(bool enabled);
}

#endif
