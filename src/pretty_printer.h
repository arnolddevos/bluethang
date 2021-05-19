#ifndef PRETTY_PRINTER_H_
#define PRETTY_PRINTER_H_

#include <mbed.h>
#include <ble/BLE.h>

int printf(const char* format, ...);
int println(const char* msg); 
int println();
int print(const char* msg);
int print_hex(int i);
void print_error(ble_error_t error, const char* msg);
void print_address(const ble::address_t &addr);

#endif /* PRETTY_PRINTER_H_ */
