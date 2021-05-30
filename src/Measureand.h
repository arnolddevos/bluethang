#pragma once
#include <mbed.h>
#include <ble/BLE.h>
#include "Measurement.h"

using namespace mbed;
using namespace ble;

struct Registration
{
    Registration(const UUID &uuid, GattCharacteristic *specs[], unsigned len) : service(uuid, specs, len) {}
    Registration(GattService s) : service(s) {}
 
    GattService service;

    void operator()(BLE* ble ){
        ble->gattServer().addService(service);
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

    void operator()(BLE* ble) 
    {
        (*ble).gattServer().write(measureand->spec.getValueHandle(), sample.buffer(), sample.length());
    }
};

template<typename D, typename A>
struct Sampler3D
{
    int (D::*read)(A& x, A& y, A& z);
    Measureand<Measurement3D<A>>* measureand;
    Scheduler<BLE>* radio;

    void operator()(D* device)
    {
        A x, y, z;
        device->*read(x, y, x);
        auto sample = Measurement3D<A>(x, y, z, 0);
        radio->submit(Update<Measurement3D<A>>(measureand, sample));
    }
};
