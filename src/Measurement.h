#pragma once

template<typename A>
struct Measurement3D
{
    Measurement3D(A x, A y, A z, unsigned short t)  {
        *ap(0) = x;
        *ap(1) = y;
        *ap(2) = z;
        *tp() = t;
    }
    Measurement3D(): Measurement3D(0, 0, 0, 0) {}

    A *ap(unsigned i) { return reinterpret_cast<A*>(_buffer + sizeof(A)*i); }
    unsigned short *tp() { return reinterpret_cast<unsigned short*>(_buffer + sizeof(A)*3); }

    unsigned char _buffer[sizeof(A)*3 + sizeof(unsigned short)];
    uint8_t* buffer() { return _buffer; } // endian - shmedian
    uint16_t length() { return sizeof(_buffer); } 

};

template<typename A>
struct Measurement
{
    Measurement(A x, unsigned short t) 
    {
        *ap() = x;
        *tp() = t;
    }

    Measurement() : Measurement(0, 0) {}

    A *ap() { return reinterpret_cast<A*>(_buffer); }
    unsigned short *tp() { return reinterpret_cast<unsigned short*>(_buffer + sizeof(A)); }

    unsigned char _buffer[sizeof(A) + sizeof(unsigned short)];
    uint8_t* buffer() { return _buffer; } // endian - shmedian
    uint16_t length() { return sizeof(_buffer); } 
};
