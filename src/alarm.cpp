#include "alarm.h"

#include <iostream>

#include "util.h"

using json = nlohmann::json;

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

nlohmann::json Alarm::toJson() const {
    nlohmann::json j;

    j["hour"] = hour;
    j["minute"] = minute;
    j["enabled"] = enabled;

    j["days"] = nlohmann::json::array();
    for (auto day: days) {
        j["days"].push_back(day);
    }

    j["lastRunDay"] = lastRunDay.has_value() ? nlohmann::json(lastRunDay.value()) : nullptr;

    return j;
}

void Alarm::fromJson(const nlohmann::json &j) {
    try {
        setTime(j.at("hour").get<int>(), j.at("minute").get<int>());
        setEnabled(j.at("enabled").get<bool>());

        days.clear();
        for (const auto &d: j.at("days")) {
            days.insert(d.get<DayOfWeek>());
        }

        if (j.contains("lastRunDay") && !j["lastRunDay"].is_null()) {
            lastRunDay = j["lastRunDay"].get<DayOfWeek>();
        } else {
            lastRunDay = std::nullopt;
        }
    } catch (const nlohmann::json::exception &e) {
        throw std::invalid_argument(std::string("Invalid alarm JSON format: ") + e.what());
    }
}

Alarm Alarm::createFromJson(const nlohmann::json& j) {
    try {
        // Example fix for a field that might be causing the issue
        std::string someField;
        if (j.contains("fieldName") && j["fieldName"].is_array()) {
            // Handle the case where fieldName is an array instead of string
            // For example, join array elements or take the first one
            auto arr = j["fieldName"].get<std::vector<std::string>>();
            someField = arr.empty() ? "" : arr[0];
        } else if (j.contains("fieldName")) {
            someField = j["fieldName"].get<std::string>();
        }

        // Rest of your implementation...
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error in createFromJson: " << e.what() << std::endl;
        // Return a default alarm or rethrow
    }
}
