#ifndef ALARM_H
#define ALARM_H

#include <unistd.h>
#include <ctime>

extern bool sound;

void startAlarm();
void stopAlarm();
bool isRunning();
void updateAlarm();
unsigned long micros();

#endif //ALARM_H