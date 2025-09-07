#include <iostream>
#include <stdexcept>

#include "application_controller.h"
#include "api.h"
#include "alarms_vector.h"
#include "buzzer.h"

int main() {
    try {
        AlarmsVector::initializeInstance("alarms.json");

        ApplicationController::initialize();
        startApiServer();

        std::cout << "Alarm initialised successfully." << std::endl;

        ApplicationController::run(); // Blocking until running is false

        stopApiServer();
        ApplicationController::shutdown();

        std::cout << "exited cleanly" << std::endl;
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
