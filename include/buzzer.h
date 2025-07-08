#ifndef BUZZER_H
#define BUZZER_H

#include <unistd.h>
#include <ctime>

extern bool sound;

void startBuzzer();

void stopBuzzer();

bool isRunning();

void updateBuzzer();

unsigned long micros();

#endif //BUZZER_H
