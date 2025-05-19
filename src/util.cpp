#include "util.h"
#include "alarm.h"

std::string toLowerString(std::string_view str) {
    std::string result(str);
    std::ranges::transform(result, result.begin(),
                           [](const unsigned char c) { return std::tolower(c); });
    return result;
}

void toJson(nlohmann::json &j, const DayOfWeek &d) {
    j = dayNames[static_cast<int>(d)];
}

void fromJson(const nlohmann::json &j, DayOfWeek &d) {
    if (j.is_number()) {
        int val = j.get<int>();
        if (val < 0 || val > 6) {
            throw std::out_of_range("Day value must be between 0 and 6");
        }
        d = static_cast<DayOfWeek>(val);
    } else if (j.is_string()) {
        const auto day_str = toLowerString(j.get<std::string>());

        for (int i = 0; i < dayNames.size(); i++) {
            if (day_str == toLowerString(dayNames[i])) {
                d = static_cast<DayOfWeek>(i);
                return;
            }
        }
        throw std::invalid_argument("Invalid day name: " + j.get<std::string>());
    } else {
        throw std::invalid_argument("Day must be an integer or string");
    }
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
