#pragma once

#include <mbed.h>
using namespace mbed;
using namespace events;
using namespace rtos;

struct Worker
{
    Worker() : queue(2048) 
    {
        thread.start(callback(&queue, &EventQueue::dispatch_forever));
    }
    ~Worker() {
        queue.break_dispatch();
    }
    Worker(const Worker&) = delete;

    EventQueue queue;
    Thread thread;

};

template<typename Driver>
class Scheduler
{
public:
    using duration = EventQueue::duration;

    Scheduler(Driver& d, Worker& w) : driver(d), queue(w.queue) {}
    Scheduler(Driver& d, EventQueue& q) : driver(d), queue(q) {}
    Scheduler(Scheduler&& s) : driver(s.driver), queue(s.queue) {}

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
