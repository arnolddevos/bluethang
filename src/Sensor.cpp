#include "Sensor.h"

Driver<IMUControl>& Sensor::driver()
{
    static IMUControl device;
    static Driver<IMUControl> inst(&device);
    device.control.begin();
    return inst;
}