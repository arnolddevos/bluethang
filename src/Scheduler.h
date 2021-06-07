#pragma once

#include <mbed.h>
using namespace mbed;
using namespace events;

template<typename Driver>
class Scheduler
{
public:
    using duration = EventQueue::duration;

    Scheduler(Driver& d, EventQueue& q) : driver(d), queue(q) {}

    template <typename F, typename... ArgTs>
    int submit(F f, ArgTs... args)
    {
        return queue.call(f, &driver, args...);
    }

    template <typename F, typename... ArgTs>
    int submit_every(duration t, F f, ArgTs... args)
    {
        return queue.call_every(t, f, &driver, args...);
    }

    template <typename F, typename... ArgTs>
    int submit_in(duration t, F f, ArgTs... args)
    {
        return queue.call_in(t, f, &driver, args...);
    }
 
 private:
    Driver& driver;
    EventQueue& queue;
};
