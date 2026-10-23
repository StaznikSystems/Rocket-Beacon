/*
 * ============================================================================
 * Project      : Beacon
 * File         : bsp_beacon.h
 * Author       : Goran Staznik
 * Organization : Staznik Systems
 * Created      : 2026 - 10 - 22
 * Description  : Board Support Package for the beacon
 *
 * Copyright (c) 2026 Goran Staznik. All rights reserved except as expressly
 * licensed below.
 *
 * LICENSE
 * Unless otherwise stated, this file is licensed under the Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
 * license.
 *
 * https://creativecommons.org/licenses/by-nc-sa/4.0/
 *
 * You may use, reproduce, modify, and redistribute this file for
 * non-commercial purposes, subject to the license terms, including attribution
 * and ShareAlike requirements.
 *
 * Commercial use, production, distribution, or sale requires separate written
 * authorization from Goran Staznik. Commercial licensing inquiries may be
 * directed to Staznik Systems.
 *
 * DISCLAIMER
 * This software is provided "AS IS", without warranties of any kind, to the
 * extent permitted by law. The user is responsible for validating its
 * suitability, reliability, and safety for the intended application.
 *
 * ============================================================================
 */

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
