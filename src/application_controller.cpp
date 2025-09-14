#include <tm1637.h>
#include <iostream>
#include <csignal>
#include <thread>
#include <atomic>
#include <memory>

#include "application_controller.h"
#include "buzzer.h"
#include "alarm.h"
#include "api.h"
#include "alarms_vector.h"
#include "util.h"
#include "button_handler.h"
#include "hardware_manager.h"
#include "pigpio.h"

// Internal state and helpers
namespace {
    std::unique_ptr<ButtonHandler> buttonHandler;

    std::atomic running{true};
    bool lastBuzzerState{false};

    auto brightnessState = BrightnessState::OFF;
    std::chrono::system_clock::time_point brightnessStartTime;

    void signalHandler(const int signum) {
        std::cout << "\nInterrupt signal received. (" << signum << ")" << std::endl;
        running.store(false, std::memory_order_relaxed);
        std::cout << "Exiting." << std::endl;
    }

    void setupSignalHandlers() {
        signal(SIGINT, signalHandler);
        gpioSetSignalFunc(SIGINT, signalHandler);
    }

    void onButtonPressed() {
        if (running.load(std::memory_order_relaxed)) {
            stopBuzzer();
        }

        tm1637SetBrightness(static_cast<unsigned char>(brightnessState));
        brightnessState = BrightnessState::FULL;
        brightnessStartTime = std::chrono::system_clock::now();
    }

    void updateBrightness() {
        if (brightnessState == BrightnessState::FULL) {
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - brightnessStartTime
            ).count();
            if (elapsed >= 10) {
                brightnessState = BrightnessState::DIM;
                tm1637SetBrightness(static_cast<unsigned char>(brightnessState));
                brightnessStartTime = std::chrono::system_clock::now();
            }
        } else if (brightnessState == BrightnessState::DIM) {
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - brightnessStartTime
            ).count();

            if (elapsed >= 30) {
                brightnessState = BrightnessState::OFF;
                tm1637SetBrightness(static_cast<unsigned char>(brightnessState));
            }
        }
    }

    void onButtonLongPressed() {
        if constexpr (!ApplicationController::SHUTDOWN_ENABLED) return;

        HardwareManager::setBuzzerState(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        running.store(false, std::memory_order_relaxed);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        system("sudo shutdown -h now");
    }

    void setupButtonCallbacks() {
        buttonHandler->setButtonPressCallback([] { onButtonPressed(); });
        buttonHandler->setLongPressCallback([] { onButtonLongPressed(); });
    }

    void handleBuzzerState() {
        if (sound != lastBuzzerState) {
            HardwareManager::setBuzzerState(sound);
            lastBuzzerState = sound;
        }
    }

    void handleAlarmsAndDisplay() {
        for (auto &alarm: AlarmsVector::getInstance().getAlarmsCopy()) {
            if (alarm.triggerAlarm()) {
                std::cout << "Alarm triggered" << std::endl;
                startBuzzer();
            }
        }
        tm1637ShowDigits(getCurrentTimeString().c_str());
    }
}

namespace ApplicationController {
    void initialize() {
        HardwareManager::initialize();

        handleAlarmsAndDisplay();
        tm1637SetBrightness(static_cast<unsigned char>(brightnessState));

        setupSignalHandlers();

        buttonHandler = std::make_unique<ButtonHandler>(HardwareManager::BUTTON_GPIO);
        setupButtonCallbacks();
    }

    void run() {
        auto nextTriggerTime = getNextMinuteTime();

        while (running.load(std::memory_order_relaxed)) {
            buttonHandler->update();
            handleBuzzerState();

            // If block runs once at the start of every minute
            if (std::chrono::system_clock::now() >= nextTriggerTime) {
                handleAlarmsAndDisplay();
                nextTriggerTime = getNextMinuteTime();
            }

            updateBrightness();
            updateBuzzer();
            std::this_thread::sleep_for(LOOP_DELAY);
        }
    }

    void shutdown() {
        running.store(false, std::memory_order_relaxed);
        stopBuzzer();

        HardwareManager::cleanup();
    }
}
