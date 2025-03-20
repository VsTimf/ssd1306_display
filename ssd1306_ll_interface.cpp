/**
  ******************************************************************************
  * @brief   SSD1306  low level communication class
  * 
  *   Default implementation for STM32 uses HAL I2C driver.
  *   You can implement your own class: it must have constructor and following methods:
  *        - void reset(void);
  *        - void ssd1306_WriteCommand(uint8_t cmd);
  *        - void ssd1306_WriteData(uint8_t* data, size_t data_size);
  *
  *        - (optional) void FillMemory(uint8_t pattern, unsigned data_size); 
*/
   

#include "ssd1306_ll_interface.hpp"


/**
 * @brief Wait while display is booting after power up
 * 
 */
void SSD1306_LL_INTERFACE::reset(void) const {
    HAL_Delay(100);                         
    /* for I2C - do nothing */
    /* for SPI - implement it yourself */
}




/**
 * @brief Sends a byte to the command register
 * 
 * @param cmd                         command to be send
 */
void SSD1306_LL_INTERFACE::WriteCommand(uint8_t cmd) const {
    while(((I2C_HandleTypeDef*)interface)->State != HAL_I2C_STATE_READY);
    HAL_I2C_Mem_Write((I2C_HandleTypeDef*)interface, address, 0x00, 1, &cmd, 1, HAL_MAX_DELAY);
}




/**
 * @brief Sends data to the ssd1306 graphical memory
 * 
 * @param data                        pointer to data to be send   
 * @param data_size                   amount of data bytes
 */
void SSD1306_LL_INTERFACE::WriteData(uint8_t* data, uint16_t data_size) const {
    while(((I2C_HandleTypeDef*)interface)->State != HAL_I2C_STATE_READY);
    HAL_I2C_Mem_Write((I2C_HandleTypeDef*)interface, address, 0x40, 1, data, data_size, 500);
}




/**
 * @brief Fills ssd1306 memory. Uses DMA
 * 
 * @param pattern                     fills memory with this pattern
 * @param data_size                   amount of data bytes
 */
void SSD1306_LL_INTERFACE::FillMemory(uint8_t pattern, unsigned data_size) const {
    static uint8_t memory_pattern = pattern;
    
    while(1)
    {
        while(((I2C_HandleTypeDef*)interface)->State != HAL_I2C_STATE_READY);

        if(data_size > 255)
        {
          HAL_I2C_Mem_Write_DMA((I2C_HandleTypeDef*)interface, address, 0x40, 1, &memory_pattern, 255);
          data_size -= 255;
        }
        else
        {
          HAL_I2C_Mem_Write_DMA((I2C_HandleTypeDef*)interface, address, 0x40, 1, &memory_pattern, data_size);
          return;
        }
    }
}