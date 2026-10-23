#ifndef BSP_BEACON_H
#define BSP_BEACON_H

#include "stm32l0xx_hal.h"
#include "stm32l0xx_it.h"

/* Pin Definitions */
#define DIO0_Pin GPIO_PIN_0
#define DIO0_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define DIO2_Pin GPIO_PIN_1
#define DIO2_GPIO_Port GPIOB
#define SX_NREST_Pin GPIO_PIN_6
#define SX_NREST_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_7
#define LED_GPIO_PORT GPIOB

/* External Hardware Handles */
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern SPI_HandleTypeDef hspi1;

/* Public BSP Functions */
void Error_Handler(void);
void BSP_Initialize(void);
void LowPower_Delay(uint32_t delay_ms);
void BSP_FLashLED(uint8_t count);

#endif /* BSP_BEACON_H */