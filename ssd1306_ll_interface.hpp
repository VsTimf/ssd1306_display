#pragma once

#include <stm32_cmsis.h>                // Include HAL Library for your mcu (#include <stm32g0xx.h> for example) in this header file 

/* Uncomment line below to use asynch DMA Transfer
 *
 * Please note, that due to differences in the peripherals, this function may not work on some series of microcontrollers 
 * (in this case you should write your own code - see how it is written for STM32G0 in .c file)
 * Also note, that due to the asynchrony of the transfer, it is possible to change the data for display before sending it to the screen controller. This will result in incorrect display on the screen
 * 
 * To use DMA - in STM32CubeMX config corresponding i2C DMA Stream and enable I2C and DMA interrupts
 */
// #define USE_DMA_TRANSFER 

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
