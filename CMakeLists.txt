cmake_minimum_required(VERSION 3.20)
project(pi_alarm)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set_source_files_properties(
        src/api.cpp
        src/alarms_vector.cpp
        PROPERTIES COMPILE_DEFINITIONS ALARMS_VECTOR_WRITE_ACCESS
)

find_package(httplib REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(pi_alarm
        src/main.cpp
        src/alarm.cpp
        src/buzzer.cpp
        src/util.cpp
        src/api.cpp
        src/alarms_vector.cpp
)

target_link_libraries(pi_alarm PRIVATE
        pigpio
        pthread
        rt
        httplib::httplib
        nlohmann_json::nlohmann_json
)

target_include_directories(pi_alarm PRIVATE
        include
        external
)
