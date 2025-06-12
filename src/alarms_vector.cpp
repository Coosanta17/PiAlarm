#include "alarms_vector.h"

#include <iterator>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>

std::unique_ptr<AlarmsVector> AlarmsVector::instance = nullptr;
std::mutex AlarmsVector::instanceMutex;

AlarmsVector::AlarmsVector(const std::string &filename) {
    if (!filename.empty()) {
        if (std::ifstream file(filename); file.is_open()) {
            try {
                nlohmann::json j;
                file >> j;
                for (const auto &alarmJson: j) {
                    Alarm alarm = Alarm::createFromJson(alarmJson);
                    alarms.push_back(alarm);
                }
            } catch (const std::exception &e) {
                std::cerr << "Error loading alarms from file during initialization: " << e.what() << std::endl;
            }
            file.close();
        }
    }
}

void AlarmsVector::initializeInstance(const std::string &filename) {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (instance == nullptr) {
        instance = std::unique_ptr<AlarmsVector>(new AlarmsVector(filename));
    } else {
        std::cerr << "Warning: AlarmsVector already initialized, ignoring call to initializeInstance" << std::endl;
    }
}

AlarmsVector &AlarmsVector::getInstance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (instance == nullptr) {
            throw std::runtime_error("AlarmsVector instance not initialized. Call initializeInstance first.");
        }
    }
    return *instance;
}

std::vector<Alarm> AlarmsVector::getAlarmsCopy() {
    std::lock_guard<std::mutex> lock(alarmsMutex);
    return alarms;
}

#ifdef ALARMS_VECTOR_WRITE_ACCESS
void AlarmsVector::setAlarms(const std::vector<Alarm> &newAlarms) {
    std::lock_guard<std::mutex> lock(alarmsMutex);
    alarms = newAlarms;
}

void AlarmsVector::addAlarm(const Alarm &alarm) {
    std::lock_guard<std::mutex> lock(alarmsMutex);
    alarms.push_back(alarm);
}

void AlarmsVector::updateAlarm(size_t index, const Alarm &alarm) {
    std::lock_guard<std::mutex> lock(alarmsMutex);
    if (index < alarms.size()) {
        alarms[index] = alarm;
    }
}

void AlarmsVector::deleteAlarm(const size_t index) {
    std::lock_guard<std::mutex> lock(alarmsMutex);
    if (index < alarms.size()) {
        alarms.erase(std::next(alarms.begin(), index));
    }
}

void AlarmsVector::loadFromFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock(alarmsMutex);
    if (std::ifstream file(filename); file.is_open()) {
        try {
            nlohmann::json j;
            file >> j;
            alarms.clear();
            for (const auto &alarmJson: j) {
                Alarm alarm = Alarm::createFromJson(alarmJson);
                alarms.push_back(alarm);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error loading alarms from file: " << e.what() << std::endl;
        }
        file.close();
    }
}

void AlarmsVector::saveToFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock(alarmsMutex);
    if (std::ofstream file(filename); file.is_open()) {
        try {
            nlohmann::json j = nlohmann::json::array();
            for (const auto &alarm: alarms) {
                j.push_back(alarm.toJson());
            }
            file << j.dump();
        } catch (const std::exception &e) {
            std::cerr << "Error saving alarms from file: " << e.what() << std::endl;
        }
        file.close();
    }
}
#endif
