#include "Radio.h"
#include "Measurement.h"
#include "Measureand.h"
#include "Sensor.h"

static Measureand<Measurement3D<float>> accel(0xA201);
static Measureand<Measurement3D<float>> magne(0xA201);
static Measureand<Measurement3D<float>> gyros(0xA201);
static GattCharacteristic* specs[] = { &accel.spec, &magne.spec, &gyros.spec };
static Registration registration(0xA200, specs, 3);

struct Sampler
{
    int (LSM9DS1Class::*read)(float& x, float& y, float& z);
    Measureand<Measurement3D<float>>* measureand;
    Scheduler<Radio>* radio;

    void operator()(Sensor* sensor)
    {
        float x, y, z;
        auto &imu = sensor->device;
        (imu.*read)(x, y, z);
        auto sample = Measurement3D<float>(x, y, z, sensor->counter++);
        radio->submit(Update<Measurement3D<float>>(measureand, sample));
    }
};

int main()
{
    Radio::scheduler().submit(registration);
    Radio::scheduler().submit(Update<Measurement3D<float>>(&accel, Measurement3D<float>(1,2,3,0)));
    return 0;
}
