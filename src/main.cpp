#include <chrono>
#include <iostream>
#include <pigpio.h>
#include <unistd.h>
#include <csignal>
#include <fstream>
#include <thread>
#include <vector>

#include "buzzer.h"
#include "alarm.h"
#include "api.h"
#include "alarms_vector.h"

#define BUZZER_GPIO 12  // PWM 0
#define BUTTON_GPIO 5
#define FREQUENCY 880  // A5 in Hz
#define PWM_DUTY_CYCLE 500000  // 50% duty cycle

#define SHUTDOWN_ENABLED true
#define LONG_PRESS_DURATION std::chrono::seconds(7)

std::thread buttonThread;

int lastButtonState = 1;
std::chrono::steady_clock::time_point buttonPressStartTime;
bool isButtonCurrentlyPressed = false;
bool longPressDetected = false;

volatile bool running = true;

void signalHandler(const int signum) {
    std::cout << "\nInterrupt signal received. (" << signum << ")" << std::endl;
    running = false;
    std::cout << "Exiting." << std::endl;
}

std::chrono::system_clock::time_point getNextMinuteTime() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto time_t_now = system_clock::to_time_t(now);
    std::tm *tm_now = std::localtime(&time_t_now);

    tm_now->tm_sec = 0;
    tm_now->tm_min += 1;
    return system_clock::from_time_t(std::mktime(tm_now));
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

void buttonLongPressed() {
    // This will have to run in buttonThread
    if constexpr (!SHUTDOWN_ENABLED) return;
    std::cout << "Button long-pressed - shutting down" << std::endl;

    gpioHardwarePWM(BUZZER_GPIO, FREQUENCY, PWM_DUTY_CYCLE);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    running = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    system("sudo shutdown -h now");
}

void buttonAndBuzzerLoop() {
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);
    int lastCheckedMinute = std::floor(nowTimeT / 60);
    bool lastBuzzerState = false;

    while (running) {
        const int buttonState = gpioRead(BUTTON_GPIO);

        if (buttonState == 0 && lastButtonState == 1) {
            buttonPressStartTime = std::chrono::steady_clock::now();
            isButtonCurrentlyPressed = true;
            buttonPressed();
        } else if (buttonState == 0 && isButtonCurrentlyPressed) {
            if (auto currentDuration = std::chrono::steady_clock::now() - buttonPressStartTime;
                currentDuration >= LONG_PRESS_DURATION && !longPressDetected) {
                longPressDetected = true;
                buttonLongPressed();
            }
        } else if (buttonState == 1 && lastButtonState == 0) {
            longPressDetected = false;
            isButtonCurrentlyPressed = false;
        }

        lastButtonState = buttonState;

        if (sound != lastBuzzerState) {
            if (sound) {
                gpioHardwarePWM(BUZZER_GPIO, FREQUENCY, PWM_DUTY_CYCLE);
            } else {
                gpioHardwarePWM(BUZZER_GPIO, 0, 0);
            }
            lastBuzzerState = sound;
        }

        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - now).count() >= 1) {
            now = std::chrono::system_clock::now();
            nowTimeT = std::chrono::system_clock::to_time_t(now);

            if (const int currentMinute = std::floor(nowTimeT / 60); currentMinute > lastCheckedMinute) {
                for (auto &alarmVector = AlarmsVector::getInstance(); auto &alarm: alarmVector.getAlarmsCopy()) {
                    if (alarm.triggerAlarm()) {
                        std::cout << "Alarm triggered" << std::endl;
                        startBuzzer();
                    }
                }
                lastCheckedMinute = currentMinute;
            }
        }

        updateBuzzer();
        usleep(10000); // 10ms
    }
}

void runLoops() {
    buttonThread = std::thread(buttonAndBuzzerLoop);
}

int main() {
    try {
        AlarmsVector::initializeInstance("alarms.json");

        initialize();
        std::cout << "Alarm initialised successfully." << std::endl;
        runLoops();

        startApiServer();

        if (buttonThread.joinable()) buttonThread.join();

        stopApiServer();

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
