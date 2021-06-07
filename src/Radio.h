#pragma once

#include <mbed.h>
#include <ble/BLE.h>
#include "Scheduler.h"

struct Radio
{
    BLE& device;
    static Scheduler<Radio>& scheduler();
    static const char* name;
private:
    Radio(BLE& ble) : device(ble) {}
};
