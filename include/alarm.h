#ifndef ALARM_H
#define ALARM_H

extern bool sound;

void startAlarm();
void stopAlarm();
bool isRunning();
void updateAlarm();
unsigned long micros();

#endif //ALARM_H