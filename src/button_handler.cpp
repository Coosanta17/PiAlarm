#include "button_handler.h"
#include <pigpio.h>
#include <iostream>

ButtonHandler::ButtonHandler(const int buttonGpio)
    : buttonGpio(buttonGpio), lastButtonState(1), isButtonCurrentlyPressed(false), longPressDetected(false) {
}

void ButtonHandler::setButtonPressCallback(const ButtonCallback &callback) {
    onButtonPress = callback;
}

void ButtonHandler::setLongPressCallback(const LongPressCallback &callback) {
    onLongPress = callback;
}

void ButtonHandler::update() {
    const int buttonState = gpioRead(buttonGpio);

    if (buttonState == 0 && lastButtonState == 1) {
        buttonPressStartTime = std::chrono::steady_clock::now();
        isButtonCurrentlyPressed = true;
        handleButtonPress();
    } else if (buttonState == 0 && isButtonCurrentlyPressed) {
        if (const auto duration = std::chrono::steady_clock::now() - buttonPressStartTime;
            duration >= LONG_PRESS_DURATION && !longPressDetected) {
            longPressDetected = true;
            handleLongPress();
        }
    } else if (buttonState == 1 && lastButtonState == 0) {
        longPressDetected = false;
        isButtonCurrentlyPressed = false;
    }

    lastButtonState = buttonState;
}

void ButtonHandler::handleButtonPress() const {
    std::cout << "Button pressed" << std::endl;
    if (onButtonPress) {
        onButtonPress();
    }
}

void ButtonHandler::handleLongPress() const {
    std::cout << "Button long-press detected" << std::endl;
    if (onLongPress) {
        onLongPress();
    }
}
