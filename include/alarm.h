#ifndef ALARM_H
#define ALARM_H

#include <ctime>
#include <optional>
#include <stdexcept>
#include <unordered_set>

enum DayOfWeek {
    Sunday = 0, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
};

template<>
struct std::hash<DayOfWeek> {
    size_t operator()(const DayOfWeek &day) const noexcept {
        return static_cast<size_t>(day);
    }
};

class Alarm {
    int hour; // 0-23
    int minute; // 0-59
    bool enabled;
    std::unordered_set<DayOfWeek> days;
    std::optional<DayOfWeek> lastRunDay;

public:
    Alarm() : hour(0), minute(0), enabled(false), days({}), lastRunDay(std::nullopt) {
    }

    Alarm(const int hour, const int minute) : hour(hour), minute(minute), enabled(true) {
        const time_t timestamp = time(nullptr);
        tm datetime = *localtime(&timestamp);
        this->days = {static_cast<DayOfWeek>(datetime.tm_wday)};
    }

    Alarm(const int hour, const int minute, const bool enabled, const std::unordered_set<DayOfWeek> &days,
          const DayOfWeek lastRunDay) : Alarm(hour, minute) {
        this->enabled = enabled;
        this->days = days;
        this->lastRunDay = lastRunDay;
    }

    void setDayEnabled(const int day, const bool enabled) {
        if (day >= 0 && day < 7) {
            const auto dayOfWeek = static_cast<DayOfWeek>(day);
            if (enabled) {
                days.insert(dayOfWeek);
            } else {
                days.erase(dayOfWeek);
            }
        } else {
            throw std::out_of_range("Day must be between 0 and 6 (Sunday = 0)");
        }
    }

    [[nodiscard]] bool isDayEnabled(const int day) const {
        return day >= 0 && day < 7 && days.contains(static_cast<DayOfWeek>(day));
    }

    [[nodiscard]] const std::unordered_set<DayOfWeek> &getEnabledDays() const {
        return days;
    }

    void setTime(const int h, const int m) {
        if (h < 0 || h >= 24) {
            throw std::out_of_range("Hour must be between 0 and 23");
        }
        if (m < 0 || m >= 60) {
            throw std::out_of_range("Minute must be between 0 and 59");
        }
        hour = h;
        minute = m;
    }

    [[nodiscard]] int getHour() const { return hour; }
    [[nodiscard]] int getMinute() const { return minute; }

    void setEnabled(const bool state) { enabled = state; }
    [[nodiscard]] bool isEnabled() const { return enabled; }

    void setLastRunDay(const DayOfWeek state) { lastRunDay = state; }
    [[nodiscard]] std::optional<DayOfWeek> getLastRunDay() const { return lastRunDay; }

    [[nodiscard]] bool shouldRing() const;
};

#endif //ALARM_H
