#pragma once
#include <mbed.h>

class Trace
{
public:
    const boolean tracing = true;

    inline void format(const char* format, ...)
    {
        if(tracing) 
        {
            va_list args;
            serial().printf(format, args);
        }
    }

    inline void operator()(const char* msg)
    {
        if(tracing)
            serial().printf("%s\r\n", msg);
    }

    inline void address(const char* msg, unsigned char* addr) 
    {
       if(tracing)
            serial().printf("%s %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                msg, addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    }

private:
    mbed::Stream& serial();
};
