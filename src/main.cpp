#include <chrono>
#include <iostream>
#include <pigpio.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <vector>
#include "../include/buzzer.h"
#include "../include/alarm.h"

#define BUZZER_GPIO 12  // PWM 0
#define BUTTON_GPIO 5
#define A5_FREQUENCY 880  // A5 in Hz
#define PWM_DUTY_CYCLE 300000  // 30% duty cycle

std::thread buttonThread;
std::thread alarmThread;

std::vector<Alarm> alarms;

int lastButtonState = 1;
volatile bool running = true;

void signalHandler(const int signum) {
    std::cout << "\nInterrupt signal received. (" << signum << ")" << std::endl;
    running = false;
    std::cout << "Exiting." << std::endl;
}

void waitUntilNextMinute() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto time_t_now = system_clock::to_time_t(now);
    std::tm *tm_now = std::localtime(&time_t_now);

    tm_now->tm_sec = 0;
    tm_now->tm_min += 1;
    const auto target = system_clock::from_time_t(std::mktime(tm_now));

    while (running && system_clock::now() < target) {
        std::this_thread::sleep_for(milliseconds(500));
    }
}

void initialize() {
    if (gpioInitialise() < 0) {
        throw std::runtime_error("PIGPIO initialization failed");
    }

    signal(SIGINT, signalHandler);
    gpioSetSignalFunc(SIGINT, signalHandler);

    gpioSetMode(BUZZER_GPIO, PI_OUTPUT);

    gpioSetMode(BUTTON_GPIO, PI_INPUT);
    gpioSetPullUpDown(BUTTON_GPIO, PI_PUD_UP);
}

void buttonPressed() {
    std::cout << "Button pressed" << std::endl;
    if (isRunning()) {
        stopBuzzer();
    }
    // TODO: `else turn display on`
}

void buttonAndBuzzerLoop() {
    while (running) {
        const int buttonState = gpioRead(BUTTON_GPIO);

        /*
         * This only detects the iteration where the button was not pressed last iteration (10ms ago) and is pressed now
         * I forgot what this did, and I'm sure others who may read this code may also be confused a bit, or maybe it is
         * just me...
         */
        if (buttonState == 0 && lastButtonState == 1) {
            buttonPressed();
        }
        lastButtonState = buttonState;

        updateBuzzer();

        if (sound) {
            gpioHardwarePWM(BUZZER_GPIO, A5_FREQUENCY, PWM_DUTY_CYCLE);
        } else {
            gpioHardwarePWM(BUZZER_GPIO, 0, 0);
        }

        usleep(10000); // 10ms
    }
}

void alarmLoop() {
    while (running) {
        for (const Alarm &alarm: alarms) {
            if (alarm.triggerAlarm()) {
                std::cout << "Alarm triggered" << std::endl;
                startBuzzer();
            }
        }

        waitUntilNextMinute();
    }
}

void runLoops() {
    buttonThread = std::thread(buttonAndBuzzerLoop);
    alarmThread = std::thread(alarmLoop);
}

void debugAlarmNotForRelease() {
    alarms.push_back(Alarm(5, 55, {Tuesday, Wednesday, Thursday}));
    alarms.push_back(Alarm(6, 55, {Monday, Friday}));
    alarms.push_back(Alarm(4, 55, {Saturday}, false));
    alarms.push_back(Alarm(21, 50, {
                               Sunday, Monday, Tuesday,
                               Wednesday, Thursday, Friday,
                               Saturday
                           }, true, Saturday));
}

int main() {
    try {
        debugAlarmNotForRelease();
        initialize();
        runLoops();

        if (buttonThread.joinable()) buttonThread.join();
        if (alarmThread.joinable()) alarmThread.join();

        stopBuzzer();
        gpioHardwarePWM(BUZZER_GPIO, 0, 0);
        gpioTerminate();

        std::cout << "exited cleanly" << std::endl;
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
