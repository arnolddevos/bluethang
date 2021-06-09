#include "Sensor.h"

using namespace mbed;
using namespace rtos;
using namespace events;

struct SensorInit
{
    SensorInit(Scheduler<Sensor>& scheduler)
    {
        scheduler.submit(*this);
    }

    void operator()(Sensor* sensor)
    {
        sensor->device.begin();
    }
};

Scheduler<Sensor>& Sensor::scheduler()
{
    static Worker worker;
    static Sensor sensor(IMU);
    static Scheduler<Sensor> scheduler(sensor, worker);
    static SensorInit init(scheduler);
    return scheduler;
}
