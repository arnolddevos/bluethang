#include "Sensor.h"

Driver<LSM9DS1Class>& Sensor::driver()
{
    static Driver<LSM9DS1Class> inst(&IMU);
    IMU.begin();
    return inst;
}