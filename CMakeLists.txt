cmake_minimum_required(VERSION 3.20)
project(PiAlarm)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(alarm_clock
        src/main.cpp
)

target_link_libraries(alarm_clock
        pigpio
        pthread
        rt
)

#if(WIN32)
#    include_directories(external)
#endif()
#
#include_directories(external)