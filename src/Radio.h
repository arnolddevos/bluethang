#include <mbed.h>
#include <ble/BLE.h>

using namespace mbed;
using namespace events;
using namespace ble;


class Radio
{    
public:
    using duration = EventQueue::duration;

    template <typename F, typename... ArgTs>
    static int call(F f, ArgTs... args)
    {
        auto &r = instance();
        return r.queue.call(f, r, args...);
    }

    template <typename F, typename... ArgTs>
    static int call_every(duration d, F f, ArgTs... args)
    {
        auto &r = instance();
        return r.queue.call_every(d, f, r, args...);
    }

    template <typename F, typename... ArgTs>
    static int call_in(duration d, F f, ArgTs... args)
    {
        auto &r = instance();
        return r.queue.call_in(d, f, r, args...);
    }

    BLE &controller;

protected:
    Radio() : controller(BLE::Instance()), queue(2048) {};
    EventQueue queue;
    static Radio& instance();

};
