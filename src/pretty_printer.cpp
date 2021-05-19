#include <mbed.h>
#include <ble/BLE.h>
#include <USBSerial.h>
#include "pretty_printer.h"

static USBSerial serial;

int printf(const char* format, ...)
{
    va_list args;
    return serial.printf(format, args);
}

int println(const char* msg) 
{
    return printf("%s\r\n", msg);
}   

int println() 
{
    return printf("\r\n");
}   

int print(const char* msg) 
{
    return printf("%s", msg);
}   

int print_hex(int i) 
{
    return printf("0x%x", i);
}   

void print_address(const ble::address_t &addr)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x\r\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}


void print_error(ble_error_t error, const char* msg)
{
    print(msg);
    switch(error) {
        case BLE_ERROR_NONE:
            println("BLE_ERROR_NONE: No error");
            break;
        case BLE_ERROR_BUFFER_OVERFLOW:
            println("BLE_ERROR_BUFFER_OVERFLOW: The requested action would cause a buffer overflow and has been aborted");
            break;
        case BLE_ERROR_NOT_IMPLEMENTED:
            println("BLE_ERROR_NOT_IMPLEMENTED: Requested a feature that isn't yet implement or isn't supported by the target HW");
            break;
        case BLE_ERROR_PARAM_OUT_OF_RANGE:
            println("BLE_ERROR_PARAM_OUT_OF_RANGE: One of the supplied parameters is outside the valid range");
            break;
        case BLE_ERROR_INVALID_PARAM:
            println("BLE_ERROR_INVALID_PARAM: One of the supplied parameters is invalid");
            break;
        case BLE_STACK_BUSY:
            println("BLE_STACK_BUSY: The stack is busy");
            break;
        case BLE_ERROR_INVALID_STATE:
            println("BLE_ERROR_INVALID_STATE: Invalid state");
            break;
        case BLE_ERROR_NO_MEM:
            println("BLE_ERROR_NO_MEM: Out of Memory");
            break;
        case BLE_ERROR_OPERATION_NOT_PERMITTED:
            println("BLE_ERROR_OPERATION_NOT_PERMITTED");
            break;
        case BLE_ERROR_INITIALIZATION_INCOMPLETE:
            println("BLE_ERROR_INITIALIZATION_INCOMPLETE");
            break;
        case BLE_ERROR_ALREADY_INITIALIZED:
            println("BLE_ERROR_ALREADY_INITIALIZED");
            break;
        case BLE_ERROR_UNSPECIFIED:
            println("BLE_ERROR_UNSPECIFIED: Unknown error");
            break;
        case BLE_ERROR_INTERNAL_STACK_FAILURE:
            println("BLE_ERROR_INTERNAL_STACK_FAILURE: internal stack failure");
            break;
        case BLE_ERROR_NOT_FOUND:
            println("BLE_ERROR_NOT_FOUND");
            break;
        default:
            println("Unknown error");
    }
}
