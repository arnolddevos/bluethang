#include "Trace.h"
#include <mbed.h>
#include <USBSerial.h>

mbed::Stream& Trace::serial() 
{
    static USBSerial inst;
    return inst;
}
