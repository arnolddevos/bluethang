#pragma once

#include <LSM9DS1.h>
#include "Scheduler.h"

class Sensor : public Scheduler<LSM9DS1Class>
{

public:
    Sensor() : Scheduler(&driver) {}

private:
    static Driver<LSM9DS1Class>& driver();

};