#ifndef UTIL_H
#define UTIL_H

#include <string_view>
#include <string>
#include <nlohmann/json.hpp>

class Alarm;

enum DayOfWeek {
    Sunday = 0, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
};

std::string toLowerString(std::string_view str);

void toJson(nlohmann::json &j, const DayOfWeek &d);

void fromJson(const nlohmann::json &j, DayOfWeek &d);

void toJson(nlohmann::json &j, const std::vector<Alarm> &v);

void fromJson(const nlohmann::json &j, std::vector<Alarm> &v);

#endif //UTIL_H
