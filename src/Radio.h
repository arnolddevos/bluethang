#include <mbed.h>
#include <ble/BLE.h>

class RadioThread;

class Radio
{
public:
    void addService(GattService &service);

private:
    RadioThread& thread();


};
