#pragma once

#include <mbed.h>
#include <ble/BLE.h>
#include "Scheduler.h"

class Radio : public Scheduler<BLE>
{    
public:
    static constexpr const char* name = "Arduino Nano";
    Radio() : Scheduler(&driver) {};
private:
    static Driver<BLE>& driver();
};
