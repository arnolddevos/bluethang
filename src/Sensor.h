#pragma once

#include <LSM9DS1.h>
#include "Measurement.h"
#include "Measureand.h"
#include "Scheduler.h"

struct IMUControl
{
    IMUControl() : control(IMU), counter(0) {}
    LSM9DS1Class& control;
    unsigned counter;
};

class Sensor : public Scheduler<IMUControl>
{
public:
    Sensor() : Scheduler(&driver) {}

private:
    static Driver<IMUControl>& driver();
};


struct IMUSampler
{
    int (LSM9DS1Class::*read)(float& x, float& y, float& z);
    Measureand<Measurement3D<float>>* measureand;
    Scheduler<BLE>* radio;

    void operator()(IMUControl* device)
    {
        float x, y, z;
        auto &imu = device->control;
        (imu.*read)(x, y, z);
        auto sample = Measurement3D<float>(x, y, z, device->counter++);
        radio->submit(Update<Measurement3D<float>>(measureand, sample));
    }
};
