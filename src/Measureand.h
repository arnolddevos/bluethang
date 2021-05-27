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

struct Register
{
    Register(const UUID &uuid, GattCharacteristic *specs[], unsigned len) : service(uuid, specs, len) {}
 
    GattService service;

    void operator()(BLE* ble ){
        ble->gattServer().addService(service);
    }
};

struct Measurement3D
{
    Measurement3D(uint16_t x, uint16_t y, uint16_t z, uint16_t t) : axis { x, y, z, t} {}
    Measurement3D() = delete;

    uint16_t axis[4];

    uint8_t* buffer() { return reinterpret_cast<uint8_t*>(axis); } // endian - shmedian
    uint16_t length() { return uint16_t(sizeof(uint16_t) * 4); } 

};

class Measureand3D
{
public:
    Measureand3D(const UUID &uuid, uint16_t x, uint16_t y, uint16_t z) : init(x, y, z, 0), last(init), spec(uuid, init.axis) {}
    Measureand3D(const UUID &uuid) : Measureand3D(uuid, 0, 0, 0) {}
    Measureand3D() = delete;

    Measurement3D init;
    Measurement3D last;
    ReadOnlyArrayGattCharacteristic<uint16_t, 4> spec;
    
    Update<Measurement3D> update(uint16_t x, uint16_t y, uint16_t z)
    {
        Measurement3D next(x, y, z, last.axis[3]+1);
        last = next;
        return Update<Measurement3D> { &spec.getValueAttribute(), next };
    }
};
