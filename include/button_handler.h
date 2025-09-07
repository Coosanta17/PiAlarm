#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <chrono>
#include <functional>

class ButtonHandler {
public:
    using ButtonCallback = std::function<void()>;
    using LongPressCallback = std::function<void()>;

    explicit ButtonHandler(int buttonGpio);

    void setButtonPressCallback(const ButtonCallback &callback);

    void setLongPressCallback(const LongPressCallback &callback);

    void update();

private:
    static constexpr std::chrono::seconds LONG_PRESS_DURATION{7};

    int buttonGpio;
    int lastButtonState;
    std::chrono::steady_clock::time_point buttonPressStartTime;
    bool isButtonCurrentlyPressed;
    bool longPressDetected;

    ButtonCallback onButtonPress;
    LongPressCallback onLongPress;

    void handleButtonPress() const;

    void handleLongPress() const;
};

#endif
