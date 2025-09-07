#include "util.h"
#include "alarm.h"
#include <ctime>

std::chrono::system_clock::time_point getNextMinuteTime() {
    const auto time_t_now = std::time(nullptr);
    std::tm *tm_now = std::localtime(&time_t_now);

    tm_now->tm_sec = 0;
    tm_now->tm_min += 1;
    return std::chrono::system_clock::from_time_t(std::mktime(tm_now));
}

std::string getCurrentTimeString() {
    const std::time_t now = std::time(nullptr);
    char buf[6];
    std::strftime(buf, sizeof(buf), "%H:%M", std::localtime(&now));
    return buf;
}


std::string toLowerString(const std::string_view str) {
    std::string result(str);
    std::ranges::transform(result, result.begin(),
                           [](const unsigned char c) { return std::tolower(c); });
    return result;
}

void toJson(nlohmann::json &j, const DayOfWeek &d) {
    j = d;
}

void fromJson(const nlohmann::json &j, DayOfWeek &d) {
    if (!j.is_number()) {
        throw std::invalid_argument("Day value must be an integer between 0 and 6 (Sunday = 0)");
    }

    int val = j.get<int>();
    if (val < 0 || val > 6) {
        throw std::out_of_range("Day value must be between 0 and 6 (Sunday = 0)");
    }
    d = static_cast<DayOfWeek>(val);
}

void toJson(nlohmann::json &j, const std::vector<Alarm> &v) {
    j = nlohmann::json::array();
    for (const auto &alarm: v) {
        j.push_back(alarm.toJson());
    }
}

void fromJson(const nlohmann::json &j, std::vector<Alarm> &v) {
    if (!j.is_array()) {
        throw std::invalid_argument("Expected JSON array for alarms");
    }

    v.clear();
    for (const auto &alarmJson: j) {
        if (!alarmJson.is_object()) {
            throw std::invalid_argument("Each alarm must be a JSON object");
        }
        v.push_back(Alarm::createFromJson(alarmJson.dump()));
    }
}
