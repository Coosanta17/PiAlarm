#include "../include/alarm.h"

bool Alarm::triggerAlarm() {
    const time_t timestamp = time(nullptr);
    const tm currentDatetime = *localtime(&timestamp);

    if (const auto today = static_cast<DayOfWeek>(currentDatetime.tm_wday);
        enabled &&
        days.contains(today) &&
        (!lastRunDay.has_value() || lastRunDay.value() != today) &&
        currentDatetime.tm_hour == hour &&
        currentDatetime.tm_min == minute) {

        setLastRunDay(today);
        return true;
    }
    return false;
}
