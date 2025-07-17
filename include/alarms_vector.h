#ifndef ALARMS_H
#define ALARMS_H
#include <vector>
#include <mutex>
#include <memory>
#include <string>

#include "alarm.h"

class AlarmsVector {
    explicit AlarmsVector(const std::string &filename = "");

    std::string filename;

    // Delete copy and move operations
    AlarmsVector(const AlarmsVector &) = delete;

    AlarmsVector &operator=(const AlarmsVector &) = delete;

    AlarmsVector(AlarmsVector &&) = delete;

    AlarmsVector &operator=(AlarmsVector &&) = delete;

    std::vector<Alarm> alarms;
    std::mutex alarmsMutex;

    static std::unique_ptr<AlarmsVector> instance;
    static std::mutex instanceMutex;

public:
    static void initializeInstance(const std::string &filename = "");

    static AlarmsVector &getInstance();

    std::vector<Alarm> getAlarmsCopy();

#ifdef ALARMS_VECTOR_WRITE_ACCESS
    void setAlarms(const std::vector<Alarm> &newAlarms);

    void addAlarm(const Alarm &alarm);

    void updateAlarm(size_t index, const Alarm &alarm);

    void deleteAlarm(size_t index);

    void loadFromFile(const std::string &filename);

    void loadFromFile();

    void saveToFile(const std::string &filename);

    void saveToFile();
#endif
};

#endif //ALARMS_H
