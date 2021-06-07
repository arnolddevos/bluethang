#include "Sensor.h"

using namespace mbed;
using namespace rtos;
using namespace events;

Scheduler<Sensor>& Sensor::scheduler()
{
    static EventQueue queue(2048);
    static Thread thread;
    static Sensor sensor(IMU);
    static Scheduler<Sensor> sched(sensor, queue);
    thread.start(callback(&queue, &EventQueue::dispatch_forever));
    queue.call(&sensor.device, &LSM9DS1Class::begin);
    return sched;
}
