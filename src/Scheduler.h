#pragma once

#include <mbed.h>
using namespace mbed;
using namespace rtos;
using namespace events;

template<typename D>
class Driver
{
public:
    Driver(D* d) : device(d), queue(2048) 
    {
        thread.start(callback(&queue, &EventQueue::dispatch_forever));
    }

    ~Driver()
    {
        queue.break_dispatch();
    }

    D* device;
    EventQueue queue;
    Thread thread;
};

template<typename D>
class Scheduler
{
public:
    Scheduler(Driver<D>& (*_driver)()) : driver(_driver) {}
    using duration = EventQueue::duration;

    template <typename F, typename... ArgTs>
    int submit(F f, ArgTs... args)
    {
        auto &d = driver();
        return d.queue.call(f, d.device, args...);
    }

    template <typename F, typename... ArgTs>
    int submit_every(duration t, F f, ArgTs... args)
    {
        auto &d = driver();
        return d.queue.call_every(t, f, d.device, args...);
    }

    template <typename F, typename... ArgTs>
    int submit_in(duration t, F f, ArgTs... args)
    {
        auto &d = driver();
        return d.queue.call_in(t, f, d.device, args...);
    }
 
private:
    Driver<D>& (*driver)();
};
