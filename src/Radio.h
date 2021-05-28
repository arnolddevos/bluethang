#include <mbed.h>
#include <ble/BLE.h>

using namespace mbed;
using namespace events;
using namespace ble;


class Radio
{    
public:
    using duration = EventQueue::duration;

    static constexpr const char* name = "Arduino Nano";

    template <typename F, typename... ArgTs>
    static int submit(F f, ArgTs... args)
    {
        auto &r = instance();
        return r.queue.call(f, &r.ble, args...);
    }

    template <typename F, typename... ArgTs>
    static int submit_every(duration d, F f, ArgTs... args)
    {
        auto &r = instance();
        return r.queue.call_every(d, f, &r.ble, args...);
    }

    template <typename F, typename... ArgTs>
    static int submit_in(duration d, F f, ArgTs... args)
    {
        auto &r = instance();
        return r.queue.call_in(d, f, &r.ble, args...);
    }
 
protected:
    Radio() : ble(BLE::Instance()), queue(2048) {};

    BLE &ble;
    EventQueue queue;
    static Radio& instance();

};
