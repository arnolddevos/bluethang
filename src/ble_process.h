
#ifndef BLE_PROCESS_H_
#define BLE_PROCESS_H_

#include <stdint.h>
#include <mbed.h>
#include "pretty_printer.h"


static const uint16_t MAX_ADVERTISING_PAYLOAD_SIZE = 50;

/**
 * Handle initialization and shutdown of the BLE Instance.
 * It will also run the  event queue and call your post init callback when everything is up and running.
 */
class BLEProcess : public ble::Gap::EventHandler
{
public:
    /**
     * Construct a BLEProcess from an event queue and a ble interface.
     * Call start() to initiate ble processing.
     */
    BLEProcess(events::EventQueue &event_queue, BLE &ble_interface, const char* name) :
        _event_queue(event_queue),
        _ble(ble_interface),
        _name(name),
        _gap(ble_interface.gap()),
        _adv_data_builder(_adv_buffer)
    {
    }

    ~BLEProcess()
    {
        stop();
    }

    /**
     * Initialize the ble interface, configure it and start advertising.
     */
    void start()
    {
        printf("Ble process started.\r\n");

        if (_ble.hasInitialized()) {
            printf("Error: the ble instance has already been initialized.\r\n");
            return;
        }

        /* handle gap events */
        _gap.setEventHandler(this);

        /* This will inform us off all events so we can schedule their handling
         * using our event queue */
        _ble.onEventsToProcess(
            makeFunctionPointer(this, &BLEProcess::schedule_ble_events)
        );

        ble_error_t error = _ble.init(
            this, &BLEProcess::on_init_complete
        );

        if (error) {
            print_error(error, "Error returned by BLE::init.\r\n");
            return;
        }

        // Process the event queue.
        _event_queue.dispatch_forever();

        return;
    }

    /**
     * Close existing connections and stop the process.
     */
    void stop()
    {
        if (_ble.hasInitialized()) {
            _ble.shutdown();
            printf("Ble process stopped.");
        }
    }

    /**
     * Subscription to the ble interface initialization event.
     *
     * @param[in] cb The callback object that will be called when the ble interface is initialized.
     */
    void on_init(mbed::Callback<void(BLE&, events::EventQueue&)> cb)
    {
        _post_init_cb = cb;
    }

    /**
     * Set callback for a succesful connection.
     *
     * @param[in] cb The callback object that will be called when we connect to a peer
     */
    void on_connect(mbed::Callback<void(BLE&, events::EventQueue&, const ble::ConnectionCompleteEvent &event)> cb)
    {
        _post_connect_cb = cb;
    }

    /** Name we advertise as. */
    const char* get_device_name()
    {
        return _name;
    }

protected:
    /**
     * Sets up adverting payload and start advertising.
     * This function is invoked when the ble interface is initialized.
     */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *event)
    {
        if (event->error) {
            print_error(event->error, "Error during the initialisation\r\n");
            return;
        }

        printf("Ble instance initialized\r\n");

        /* All calls are serialised on the user thread through the event queue */
        start_activity();

        if (_post_init_cb) {
            _post_init_cb(_ble, _event_queue);
        }
    }

    /**
     * Start the gatt client process when a connection event is received.
     * This is called by Gap to notify the application we connected
     */
    void onConnectionComplete(const ble::ConnectionCompleteEvent &event) override
    {
        if (event.getStatus() == BLE_ERROR_NONE) {
            printf("Connected to: ");
            print_address(event.getPeerAddress());
            if (_post_connect_cb) {
                _post_connect_cb(_ble, _event_queue, event);
            }
        } else {
            printf("Failed to connect\r\n");
            start_activity();
        }
    }

    /**
     * Stop the gatt client process when the device is disconnected then restart advertising.
     * This is called by Gap to notify the application we disconnected
     */
    void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) override
    {
        printf("Disconnected.\r\n");
        start_activity();
    }

    /** Restarts main activity */
    void onAdvertisingEnd(const ble::AdvertisingEndEvent &event)
    {
        start_activity();
    }

    /**
     * Start advertising or scanning. Triggered by init or disconnection.
     */
    virtual void start_activity()
    {
        _event_queue.call([this]() { start_advertising(); });
    }

    /**
     * Start the advertising process; it ends when a device connects.
     */
    void start_advertising()
    {
        ble_error_t error;

        if (_gap.isAdvertisingActive(_adv_handle)) {
            /* we're already advertising */
            return;
        }

        ble::AdvertisingParameters adv_params(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(40))
        );

        error = _gap.setAdvertisingParameters(_adv_handle, adv_params);

        if (error) {
            printf("_ble.gap().setAdvertisingParameters() failed\r\n");
            return;
        }

        _adv_data_builder.clear();
        _adv_data_builder.setFlags();
        _adv_data_builder.setName(get_device_name());

        /* Set payload for the set */
        error = _gap.setAdvertisingPayload(
            _adv_handle, _adv_data_builder.getAdvertisingData()
        );

        if (error) {
            print_error(error, "Gap::setAdvertisingPayload() failed\r\n");
            return;
        }

        error = _gap.startAdvertising(_adv_handle, ble::adv_duration_t(ble::millisecond_t(4000)));

        if (error) {
            print_error(error, "Gap::startAdvertising() failed\r\n");
            return;
        }

        printf("Advertising as \"%s\"\r\n", get_device_name());
    }

    /**
     * Schedule processing of events from the BLE middleware in the event queue.
     */
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event)
    {
        _event_queue.call(mbed::callback(&event->ble, &BLE::processEvents));
    }

protected:
    events::EventQueue &_event_queue;
    BLE &_ble;
    const char* _name;
    ble::Gap &_gap;

    uint8_t _adv_buffer[MAX_ADVERTISING_PAYLOAD_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;

    ble::advertising_handle_t _adv_handle = ble::LEGACY_ADVERTISING_HANDLE;

    mbed::Callback<void(BLE&, events::EventQueue&)> _post_init_cb;
    mbed::Callback<void(BLE&, events::EventQueue&, const ble::ConnectionCompleteEvent &event)> _post_connect_cb;
};

#endif /* BLE_PROCESS_H_ */
