#ifndef APPLICATION_CONTROLLER_H
#define APPLICATION_CONTROLLER_H

#include <chrono>

enum class BrightnessState {
    OFF = 0,
    DIM = 1,
    FULL = 8
};

namespace ApplicationController {
    inline constexpr bool SHUTDOWN_ENABLED = true;
    inline constexpr std::chrono::milliseconds LOOP_DELAY{50};

    void initialize();
    void run();
    void shutdown();
}

#endif
