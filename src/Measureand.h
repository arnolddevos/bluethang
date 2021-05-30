#pragma once
#include <mbed.h>
#include <ble/BLE.h>
#include "Measurement.h"

using namespace mbed;
using namespace ble;

template<typename M>
struct Update
{
    Update() = delete;
    
    GattAttribute* attr;
    M value;

    void operator()(BLE* ble) 
    {
        (*ble).gattServer().write(attr->getHandle(), value.buffer(), value.length());
    }
};

struct Registration
{
    Registration(const UUID &uuid, GattCharacteristic *specs[], unsigned len) : service(uuid, specs, len) {}
    Registration(GattService s) : service(s) {}
 
    GattService service;

    void operator()(BLE* ble ){
        ble->gattServer().addService(service);
    }
};

template<typename A>
class Measureand3D
{
public:
    Measureand3D(const UUID &uuid, A x, A y, A z) : 
        init(x, y, z, 0), 
        last(init), 
        spec(
            uuid,
            init.buffer(),
            init.length(),
            init.length(),
            GattCharacteristic::Properties_t::BLE_GATT_CHAR_PROPERTIES_READ,
            nullptr,
            0,
            false) {}
    Measureand3D(const UUID &uuid) : Measureand3D(uuid, 0, 0, 0) {}
    Measureand3D() = delete;

    Measurement3D<A> init;
    Measurement3D<A> last;
    GattCharacteristic spec;

    uint16_t count() { return *last.tp(); }
    
    Update<Measurement3D<A>> update(A x, A y, A z)
    {
        Measurement3D<A> next(x, y, z, count()+1);
        last = next;
        return Update<Measurement3D<A>> { &spec.getValueAttribute(), next };
    }
};

template<typename D, typename A>
struct Sampler3D
{
    int (D::*read)(A& x, A& y, A& z);
    Measureand3D<A>* measureand;
    Scheduler<BLE>* radio;

    void operator()(D* device)
    {
        A x, y, z;
        device->*read(x, y, x);
        auto u = measureand->update(*x, *y, *z);
        radio->submit(u);
    }
};
