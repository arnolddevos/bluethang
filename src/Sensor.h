#pragma once

#include <LSM9DS1.h>
#include "Scheduler.h"

class Sensor
{
public:
    LSM9DS1Class& device;
    unsigned counter;
    static Scheduler<Sensor>& scheduler();
private:
    Sensor(LSM9DS1Class& d) : device(d), counter(0) {}
    Sensor(const Sensor&) = delete;
    Sensor() = delete;
};
