#include "Radio.h"
#include <ble/BLE.h>
#include "Trace.h"

using namespace mbed;
using namespace rtos;
using namespace events;
using namespace ble;

static Trace trace;

struct RadioAdvert
{
    RadioAdvert(const char* name) : advertName(name) {}
    RadioAdvert() : advertName(nullptr) {}
    const char* advertName;
    void operator()(Radio*);
};

struct RadioEvents;

struct RadioInit
{
    RadioInit(Scheduler<Radio>& scheduler, RadioEvents* e) : events(e) 
    {
        scheduler.submit(*this);
    }
    RadioEvents* events;
    void operator()(Radio*);
};

struct RadioProcess
{
    void operator()(Radio*);
};

struct RadioEvents : Gap::EventHandler
{
    RadioEvents(Scheduler<Radio>& s) : scheduler(s) {}

    Scheduler<Radio>& scheduler;
    
    void ready(BLE::InitializationCompleteCallbackContext *context)
    {
        if(context && context->error)
        {
            trace.format("BLE failed to initialise with %x", context->error);
            return;
        }
        else
        {
            trace("BLE initialised");
            scheduler.submit(RadioAdvert());
        }
    }

    void onConnectionComplete(const ConnectionCompleteEvent &event) override
    {
        if(event.getStatus()) 
        {
            trace("Failed connection");
            scheduler.submit(RadioAdvert());
        }
        else
            trace("Conection opened");
    }

    void onDisconnectionComplete(const DisconnectionCompleteEvent &event) override
    {
        trace("Connection closed");
        scheduler.submit(RadioAdvert());
    }

    void onAdvertisingStart(const AdvertisingStartEvent &event) override
    {
        trace("Advertising started");
    }

    void onAdvertisingEnd(const AdvertisingEndEvent &event) override
    {
        trace("Advertising stopped");
        // scheduler.submit(RadioAdvert());
    }

    void eventToProcess(BLE::OnEventsToProcessCallbackContext *context)
    {
        scheduler.submit(RadioProcess());
    }
};

void RadioProcess::operator()(Radio* radio)
{
    radio->device.processEvents();
}

void RadioInit::operator()(Radio* radio)
{
    auto &device = radio->device;
    device.onEventsToProcess(makeFunctionPointer(events, &RadioEvents::eventToProcess));
    device.gap().setEventHandler(events);

    if(device.hasInitialized())
    { 
        trace("BLE already initialised");
        events->ready(nullptr);
    }
    else 
    {
        trace("Requesting BLE initialisation");
        device.init(events, &RadioEvents::ready);
    }
}

void RadioAdvert::operator()(Radio* radio)
{
    ble_error_t error;

    auto &gap = radio->device.gap();
    auto handle = LEGACY_ADVERTISING_HANDLE;
    auto name =  advertName ? advertName : radio->advertName;

    if(! name )
    {
        trace("Advertising not started, no name given");
        return;
    }

    if (gap.isAdvertisingActive(handle)) 
    {
        trace("Advertising will be stopped and restarted");
        gap.stopAdvertising(handle);
    }

    AdvertisingParameters params(
        advertising_type_t::CONNECTABLE_UNDIRECTED,
        adv_interval_t(millisecond_t(40))
    );

    error = gap.setAdvertisingParameters(handle, params);

    if (error) {
        trace.format("setAdvertisingParameters() failed with %x\r\n", error);
        return;
    }

    AdvertisingDataBuilder builder(radio->advertBuffer);
    builder.clear();
    builder.setFlags();
    builder.setName(name);

    /* Set payload for the set */
    error = gap.setAdvertisingPayload( handle, builder.getAdvertisingData());

    if (error) {
        trace.format("setAdvertisingPayload() failed with %x\r\n", error);
        return;
    }

    error = gap.startAdvertising(handle);

    if (error) {
        trace.format("startAdvertising() failed with %x\r\n", error);
        return;
    }

    radio->advertName = name;

    trace.format("Advertising as: %s\r\n", name);
}

Scheduler<Radio>& Radio::scheduler()
{
    static Worker worker;
    static Radio radio( BLE::Instance() );
    static Scheduler<Radio> scheduler(radio, worker);
    static RadioEvents events(scheduler);
    static RadioInit init(scheduler, &events);

    return scheduler;
}
