cmake_minimum_required(VERSION 3.20)
project(pi_alarm)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(httplib REQUIRED)

add_executable(pi_alarm
        src/main.cpp
        src/alarm.cpp
        src/api.cpp
)

target_link_libraries(pi_alarm PRIVATE
        pigpio
        pthread
        rt
        httplib::httplib
)

target_include_directories(pi_alarm PRIVATE
        include
        external
)