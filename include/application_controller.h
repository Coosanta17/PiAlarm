#ifndef APPLICATION_CONTROLLER_H
#define APPLICATION_CONTROLLER_H

#include <chrono>

namespace ApplicationController {
    inline constexpr bool SHUTDOWN_ENABLED = true;
    inline constexpr std::chrono::milliseconds LOOP_DELAY{50};

    void initialize();
    void run();
    void shutdown();
}

#endif
