#pragma once
#include <mbed.h>
#include <ble/BLE.h>

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
struct Measurement3D
{
    Measurement3D(A x, A y, A z, unsigned short t)  {
        *ap(0) = x;
        *ap(1) = y;
        *ap(2) = z;
        *tp() = t;
    }
    Measurement3D() = delete;

    unsigned char _buffer[sizeof(A)*3 + sizeof(unsigned short)];

    A *ap(unsigned i) { return reinterpret_cast<A*>(_buffer + sizeof(A)*i); }
    unsigned short *tp() { return reinterpret_cast<unsigned short*>(_buffer + sizeof(A)*3); }

    uint8_t* buffer() { return _buffer; } // endian - shmedian
    uint16_t length() { return sizeof(_buffer); } 

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
    
    Update<Measurement3D<A>> update(A x, A y, A z)
    {
        Measurement3D<A> next(x, y, z, (*last.tp())+1);
        last = next;
        return Update<Measurement3D<A>> { &spec.getValueAttribute(), next };
    }
};
