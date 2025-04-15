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
    HAL_I2C_Mem_Write((I2C_HandleTypeDef*)interface, address, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, HAL_MAX_DELAY);
}




/**
 * @brief Sends data to the ssd1306 graphical memory
 * 
 * @param data                        pointer to data to be send   
 * @param data_size                   amount of data bytes
 */
void SSD1306_LL_INTERFACE::WriteData(uint8_t* data, uint16_t data_size) const {
    while(((I2C_HandleTypeDef*)interface)->State != HAL_I2C_STATE_READY);

    #ifndef USE_DMA_TRANSFER
    HAL_I2C_Mem_Write((I2C_HandleTypeDef*)interface, address, 0x40, I2C_MEMADD_SIZE_8BIT, data, data_size, 500);

    #else

    #ifdef STM32G0
    ((I2C_HandleTypeDef*)interface)->hdmatx->Instance->CCR &= ~DMA_CCR_EN;
    ((I2C_HandleTypeDef*)interface)->hdmatx->Instance->CCR |= DMA_CCR_MINC;
    ((I2C_HandleTypeDef*)interface)->hdmatx->Instance->CCR |= DMA_CCR_EN;
    #else
    ((I2C_HandleTypeDef*)interface)->hdmatx->Instance->CR |= DMA_SxCR_MINC;
    #endif
    
    HAL_I2C_Mem_Write_DMA((I2C_HandleTypeDef*)interface, address, 0x40, I2C_MEMADD_SIZE_8BIT, data, data_size);
    
    #endif

}




/**
 * @brief Fills ssd1306 memory with specified pattern. Uses DMA
 * 
 * @param pattern                     fills memory with this pattern
 * @param data_size                   amount of data bytes
 */
void SSD1306_LL_INTERFACE::FillMemory(uint8_t pattern, unsigned data_size) const {
    static uint8_t memory_pattern;

    #ifndef USE_DMA_TRANSFER
    return;
    #endif
    
    while(((I2C_HandleTypeDef*)interface)->State != HAL_I2C_STATE_READY);
    memory_pattern = pattern;

    

    #ifdef STM32G0
    ((I2C_HandleTypeDef*)interface)->hdmatx->Instance->CCR &= ~DMA_CCR_EN;
    ((I2C_HandleTypeDef*)interface)->hdmatx->Instance->CCR &= ~DMA_CCR_MINC;
    ((I2C_HandleTypeDef*)interface)->hdmatx->Instance->CCR |= DMA_CCR_EN;

    while(1)
    {
      if(data_size > 255)
      {
        while(((I2C_HandleTypeDef*)interface)->State != HAL_I2C_STATE_READY);
        HAL_I2C_Mem_Write_DMA((I2C_HandleTypeDef*)interface, address, 0x40, I2C_MEMADD_SIZE_8BIT, &memory_pattern, 255);
        data_size -= 255;
      }
      else
      {
        while(((I2C_HandleTypeDef*)interface)->State != HAL_I2C_STATE_READY);
        HAL_I2C_Mem_Write_DMA((I2C_HandleTypeDef*)interface, address, 0x40, I2C_MEMADD_SIZE_8BIT, &memory_pattern, data_size);
        return;
      }
    }
  

    #else
    ((I2C_HandleTypeDef*)interface)->hdmatx->Instance->CR &= ~DMA_SxCR_MINC;
    HAL_I2C_Mem_Write_DMA((I2C_HandleTypeDef*)interface, address, 0x40, I2C_MEMADD_SIZE_8BIT, &memory_pattern, data_size);

    #endif
}