#ifndef BUZZER_H
#define BUZZER_H

#include <unistd.h>
#include <ctime>

extern bool sound;

void startAlarm();
void stopAlarm();
bool isRunning();
void updateAlarm();
unsigned long micros();

#endif //BUZZER_H