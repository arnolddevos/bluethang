#include "Radio.h"
#include <ble/BLE.h>
#include "Trace.h"

using namespace mbed;
using namespace rtos;
using namespace events;
using namespace ble;

static Trace trace;

struct RadioThread : Gap::EventHandler, GattServer::EventHandler, Driver<BLE>
{
    RadioThread() : Driver(&BLE::Instance())
    {
        if((*device).hasInitialized())
        { 
            trace("BLE controller already initialised");
            ready(nullptr);
        }
        else 
            (*device).init(this, &RadioThread::ready);
    }

    ~RadioThread()
    {
        (*device).shutdown();
    }

    Thread thread;
    uint8_t buffer[50];

    void ready(BLE::InitializationCompleteCallbackContext *context)
    {
        if(context && context->error)
        {
            trace.format("BLE controller failed to initialise with %x", context->error);
            return;
        }
        thread.start(callback(this, &RadioThread::main));
    }

    void main()
    {
        trace("BLE thread running");
        (*device).onEventsToProcess(makeFunctionPointer(this, &RadioThread::eventToProcess));
        (*device).gap().setEventHandler(this);
        startAdvertising();
        queue.dispatch_forever();

    }

    void onConnectionComplete(const ConnectionCompleteEvent &event) override
    {
        if(event.getStatus()) 
        {
            trace("Failed connection");
            queue.call(this, &RadioThread::startAdvertising);
        }
        else
            trace("Conection opened");
    }

    void onDisconnectionComplete(const DisconnectionCompleteEvent &event) override
    {
        trace("Connection closed");
        queue.call(this, &RadioThread::startAdvertising);
    }

    void onAdvertisingStart(const AdvertisingStartEvent &event) override
    {
        trace("Advertising started");
    }

    void onAdvertisingEnd(const AdvertisingEndEvent &event) override
    {
        trace("Advertising stopped");
        queue.call(this, &RadioThread::startAdvertising);
    }

    void startAdvertising()
    {
        ble_error_t error;

        auto &gap = (*device).gap();
        auto handle = LEGACY_ADVERTISING_HANDLE;

        if (gap.isAdvertisingActive(handle)) {
            trace("Advertising already started");
            return;
        }

        AdvertisingParameters params(
            advertising_type_t::CONNECTABLE_UNDIRECTED,
            adv_interval_t(millisecond_t(40))
        );

        error = gap.setAdvertisingParameters(handle, params);

        if (error) {
            trace.format("_ble.gap().setAdvertisingParameters() failed with %x\r\n", error);
            return;
        }

        AdvertisingDataBuilder builder(buffer);
        builder.clear();
        builder.setFlags();
        builder.setName(Radio::name);

        /* Set payload for the set */
        error = gap.setAdvertisingPayload( handle, builder.getAdvertisingData());

        if (error) {
            trace.format("Gap::setAdvertisingPayload() failed with %x\r\n", error);
            return;
        }

        error = gap.startAdvertising(handle, adv_duration_t(millisecond_t(4000)));

        if (error) {
            trace.format("Gap::startAdvertising() failed with %x\r\n", error);
            return;
        }

        trace.format("Advertising as: %s\r\n", Radio::name);
    }

    void eventToProcess(BLE::OnEventsToProcessCallbackContext *context)
    {
        queue.call(device, &BLE::processEvents);
    }
};

Driver<BLE>& Radio::driver()
{
    static RadioThread inst;
    return inst;
}
