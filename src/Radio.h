#pragma once

#include <mbed.h>
#include <ble/BLE.h>
#include "Scheduler.h"

class Radio
{
public:
    BLE& device;
    uint8_t advertBuffer[50];
    const char* advertName;
    static Scheduler<Radio>& scheduler();
private:
    Radio(BLE& ble) : device(ble), advertName("Arduino Nano BLE") {}
    Radio(const Radio&) = delete;
    Radio() = delete;
};
