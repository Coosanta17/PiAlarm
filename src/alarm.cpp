#include "../include/alarm.h"

bool Alarm::triggerAlarm() const {
    const time_t timestamp = time(nullptr);

    if (tm currentDatetime = *localtime(&timestamp);
        enabled &&
        days.contains(static_cast<DayOfWeek>(currentDatetime.tm_wday)) &&
        lastRunDay != currentDatetime.tm_wday &&
        currentDatetime.tm_hour == hour &&
        currentDatetime.tm_min == minute) {
        return true;
    }
    return false;
}
