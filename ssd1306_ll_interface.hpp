#pragma once

#include <stm32g0xx.h>                // Specify HAL Library for your controller


class SSD1306_LL_INTERFACE
{
private:
    const void *interface;
    const uint8_t address;
        
public:
    SSD1306_LL_INTERFACE(void *_interface, uint8_t _address) : interface(_interface), address(_address){}

    void reset (void) const;
    void WriteCommand(uint8_t cmd) const;
    void WriteData(uint8_t* data, uint16_t data_size) const;
    void FillMemory(uint8_t pattern, unsigned data_size) const;
};