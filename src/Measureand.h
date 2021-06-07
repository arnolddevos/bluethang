#pragma once
#include <mbed.h>
#include <ble/BLE.h>
#include "Measurement.h"
#include "Radio.h"

using namespace mbed;
using namespace ble;

struct Registration
{
    Registration(const UUID &uuid, GattCharacteristic *specs[], unsigned len) : service(uuid, specs, len) {}
    Registration(GattService s) : service(s) {}
 
    GattService service;

    void operator()(Radio* radio ){
        radio->device.gattServer().addService(service);
    }
};

template<typename M>
class Measureand
{
public:
    Measureand(const UUID &uuid) : 
        spec(
            uuid,
            init.buffer(),
            init.length(),
            init.length(),
            GattCharacteristic::Properties_t::BLE_GATT_CHAR_PROPERTIES_READ,
            nullptr,
            0,
            false) {}
    Measureand() = delete;

    M init;
    GattCharacteristic spec;
};

template<typename M>
struct Update
{
    Update(Measureand<M>* m, const M &s) : measureand(m), sample(s) {}
    Update() = delete;
    
    Measureand<M>* measureand;
    M sample;

    void operator()(Radio* radio) 
    {
        radio->device.gattServer().write(measureand->spec.getValueHandle(), sample.buffer(), sample.length());
    }
};
