#ifndef ALARM_H
#define ALARM_H

#include <ctime>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include "util.h"

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

    void fromJson(const nlohmann::json &j);

public:
    explicit Alarm() : hour(0), minute(0), enabled(false), days({}), lastRunDay(std::nullopt) {
    }

    explicit Alarm(const int hour, const int minute) : hour(hour), minute(minute), enabled(true) {
        const time_t timestamp = time(nullptr);
        tm datetime = *localtime(&timestamp);
        this->days = {static_cast<DayOfWeek>(datetime.tm_wday)}; // Today
    }

    explicit Alarm(const int hour, const int minute, const std::unordered_set<DayOfWeek> &days) : Alarm(hour, minute) {
        this->days = days;
        this->enabled = true;
    }

    explicit Alarm(const int hour, const int minute, const std::unordered_set<DayOfWeek> &days, const bool enabled)
        : Alarm(hour, minute, days) {
        this->enabled = enabled;
    }

    explicit Alarm(const int hour, const int minute, const std::unordered_set<DayOfWeek> &days, const bool enabled,
                   const DayOfWeek lastRunDay) : Alarm(hour, minute, days, enabled) {
        this->lastRunDay = lastRunDay;
    }

    explicit Alarm(const nlohmann::json &j) : Alarm() {
        fromJson(j);
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

    [[nodiscard]] std::unordered_set<DayOfWeek> getEnabledDays() const {
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

    [[nodiscard]] bool triggerAlarm();

    [[nodiscard]] nlohmann::json toJson() const;

    static Alarm createFromJson(const nlohmann::json &j);
};

#endif //ALARM_H
