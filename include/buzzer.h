#ifndef BUZZER_H
#define BUZZER_H

#include <unistd.h>
#include <ctime>
#include <mutex>

extern bool sound;
extern bool on;

void startBuzzer();

void stopBuzzer();

bool isRunning();

void updateBuzzer();

unsigned long micros();

#endif //BUZZER_H
