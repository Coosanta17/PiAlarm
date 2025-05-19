#include "util.h"
#include "alarm.h"

std::string toLowerString(std::string_view str) {
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
