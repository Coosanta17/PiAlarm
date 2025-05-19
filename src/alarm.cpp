#include "alarm.h"
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

Alarm Alarm::createFromJson(const std::string &jsonStr) {
    try {
        const nlohmann::json j = nlohmann::json::parse(jsonStr);
        return Alarm(j);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::invalid_argument(std::string("Invalid JSON string: ") + e.what());
    }
}
