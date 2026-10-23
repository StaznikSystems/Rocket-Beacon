/*
 * ============================================================================
 * Project      : Beacon
 * File         : sx1276_driver.h
 * Author       : Goran Staznik
 * Organization : Staznik Systems
 * Created      : 2026 - 10 - 22
 * Description  : Header file for the custom SX1276 driver
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

#ifndef SX1276_DRIVER_H

#define SX1276_DRIVER_H
#include "main.h"

/* Morse Timing Definitions (in milliseconds) */
#define MORSE_DOT_MS (1200U / BEACON_MORSE_WPM)
#define MORSE_DASH_MS (MORSE_DOT_MS * 3U)
#define MORSE_ELEM_SPACE_MS (MORSE_DOT_MS)
#define MORSE_LETTER_SPACE_MS (MORSE_DOT_MS * 3U)
#define MORSE_WORD_SPACE_MS (MORSE_DOT_MS * 7U)

/**
 * @brief Status codes returned by all SX1276 driver functions for execution tracking.
 */
typedef enum
{
    SX1276_STATUS_OK = 0x00U,          // Operation executed successfully
    SX1276_STATUS_ERROR = 0x01U,       // General error or hardware verification mismatch
    SX1276_STATUS_ERR_PARAM = 0x02U,   // Invalid input parameter (e.g. NULL pointer or 0 length)
    SX1276_STATUS_ERR_TIMEOUT = 0x03U, // SPI hardware operation timed out
    SX1276_STATUS_ERR_BUSY = 0x04U,    // Peripheral bus busy
    SX1276_STATUS_ERR_SPI = 0x05U      // SPI bus communication failure
} sx1276_status_t;

/**
 * @brief Hardware handle configuration structure for the SX1276 device instance.
 */
typedef struct
{
    SPI_HandleTypeDef *hspi;  // 32-bit pointer (offset 0)
    GPIO_TypeDef *cs_port;    // 32-bit pointer (offset 4)
    GPIO_TypeDef *reset_port; // 32-bit pointer (offset 8)
    GPIO_TypeDef *dio2_port;  // 32-bit pointer (offset 12)
    uint32_t spi_timeout_ms;  // 32-bit integer (offset 16)
    uint16_t cs_pin;          // 16-bit integer (offset 20)
    uint16_t reset_pin;       // 16-bit integer (offset 22)
    uint16_t dio2_pin;        // 16-bit integer (offset 24)
    uint16_t reserved;        // 16-bit explicit padding (offset 26)
} sx1276_handle_t;

/* --- FUNCTION PROTOTYPES & USAGE EXAMPLES --- */

/**
 * @brief Writes a single byte value to a specified SX1276 register over SPI.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param reg Target register address byte.
 * @param value Byte value to write to the register.
 * @retval sx1276_status_t Execution status code.
 *
 * @code
 * // Example: Write 0x00 to RegOpMode (0x01) to put radio in Sleep mode
 * sx1276_status_t status = SX1276_WriteRegister(&beacon_radio, 0x01U, 0x00U);
 * @endcode
 */
sx1276_status_t SX1276_WriteRegister(sx1276_handle_t *const p_dev, const uint8_t reg, const uint8_t value);

/**
 * @brief Writes a byte to a register and immediately reads it back over SPI to verify the write operation.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param reg Target register address byte.
 * @param value Byte value expected to be written and verified.
 * @retval sx1276_status_t Returns SX1276_STATUS_OK if write and readback match; SX1276_STATUS_ERROR on mismatch.
 *
 * @code
 * // Example: Write and confirm carrier frequency MSB (0x06) to 0x24
 * sx1276_status_t status = SX1276_WriteAndConfirm(&beacon_radio, 0x06U, 0x24U);
 * @endcode
 */
sx1276_status_t SX1276_WriteAndConfirm(sx1276_handle_t *const p_dev, const uint8_t reg, const uint8_t value);

/**
 * @brief Reads a single byte value from a specified SX1276 register over SPI.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param reg Target register address byte.
 * @param p_value Pointer to the destination variable where the read byte will be stored.
 * @retval sx1276_status_t Execution status code.
 *
 * @code
 * // Example: Read silicon revision from RegVersion (0x42)
 * uint8_t chip_version = 0U;
 * sx1276_status_t status = SX1276_ReadRegister(&beacon_radio, 0x42U, &chip_version);
 * @endcode
 */
sx1276_status_t SX1276_ReadRegister(sx1276_handle_t *const p_dev, const uint8_t reg, uint8_t *const p_value);

sx1276_status_t SX1276_ReadModifyWriteConfirm(sx1276_handle_t *const p_dev, uint8_t reg, uint8_t mask, uint8_t value);

/**
 * @brief Writes a sequence of bytes to a starting register (e.g., FIFO) in burst mode while holding CS low.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param reg Starting target register address byte.
 * @param p_tx_buffer Pointer to the array containing bytes to send.
 * @param length Number of bytes to transmit.
 * @retval sx1276_status_t Execution status code.
 *
 * @code
 * // Example: Write a 4-byte buffer into the FIFO register (0x00)
 * uint8_t payload[4] = {0x01, 0x02, 0x03, 0x04};
 * sx1276_status_t status = SX1276_WriteBuffer(&beacon_radio, 0x00U, payload, 4U);
 * @endcode
 */
sx1276_status_t SX1276_WriteBuffer(sx1276_handle_t *const p_dev, const uint8_t reg, const uint8_t *const p_tx_buffer, const uint16_t length);

/**
 * @brief Reads a sequence of bytes from a starting register in burst mode into a buffer while holding CS low.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param reg Starting target register address byte.
 * @param p_rx_buffer Pointer to the memory buffer where incoming bytes will be stored.
 * @param length Number of bytes to read.
 * @retval sx1276_status_t Execution status code.
 *
 * @code
 * // Example: Read 8 bytes from FIFO register (0x00) into a destination array
 * uint8_t rx_data[8] = {0};
 * sx1276_status_t status = SX1276_ReadBuffer(&beacon_radio, 0x00U, rx_data, 8U);
 * @endcode
 */
sx1276_status_t SX1276_ReadBuffer(sx1276_handle_t *const p_dev, const uint8_t reg, uint8_t *const p_rx_buffer, const uint16_t length);

/**
 * @brief Resets hardware, verifies silicon version (0x12), sets carrier frequency, RFO low power, and continuous FSK mode.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @retval sx1276_status_t Execution status code.
 *
 * @code
 * // Example: Initialize radio hardware during setup
 * if (SX1276_STATUS_OK == SX1276_Init(&beacon_radio))
 * {
 *     // Radio initialized successfully and ready for beaconing
 * }
 * @endcode
 */
sx1276_status_t SX1276_Init(sx1276_handle_t *const p_dev);

/**
 * @brief Translates an alphanumeric character into its corresponding Morse code string pattern.
 * @param c Alphanumeric character to look up ('A'-'Z', '0'-'9', ' ').
 * @retval const char* Pointer to dot/dash string pattern (e.g. ".-" for 'A'), or NULL if character is invalid.
 *
 * @code
 * // Example: Retrieve Morse pattern for letter 'G'
 * const char *pattern = SX1276_GetMorsePattern('G'); // Returns pointer to "--."
 * @endcode
 */
const char *SX1276_GetMorsePattern(const char c);

/**
 * @brief Generates an audio tone on DIO2 at the handle's configured tone_freq_hz for a requested duration.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param duration_ms Playback duration in milliseconds.
 *
 * @code
 * // Example: Play an audio tone on DIO2 at configured frequency for 240 milliseconds
 * SX1276_PlayTone(&beacon_radio, 240U);
 * @endcode
 */
void SX1276_PlayTone(sx1276_handle_t *const p_dev, const uint16_t duration_ms);

/**
 * @brief Keys the RF transmitter and plays a Morse code string message over FM, then returns the radio to Sleep mode.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param p_message Null-terminated string containing the text to transmit (e.g. "VA3XXX").
 * @retval sx1276_status_t Execution status code.
 *
 * @code
 * // Example: Transmit callsign "VA3XXX" in Morse code over FM
 * sx1276_status_t status = SX1276_TransmitMorse(&beacon_radio, BEACON_CALLSIGN);
 * @endcode
 */
sx1276_status_t SX1276_TransmitMorse(sx1276_handle_t *const p_dev, const char *const p_message);

#endif /* SX1276_DRIVER_H */
