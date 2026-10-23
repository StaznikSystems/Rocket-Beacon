#include "sx1276_driver.h"
#include "main.h"

/**
 * @brief Sets a GPIO pin high using the BSRR register.
 * @param port Pointer to GPIO port structure.
 * @param pin Pin mask to set high.
 */
static inline void GPIO_SetPin(GPIO_TypeDef *const port, uint16_t pin)
{
    if (NULL != port) // validate gpio port pointer
    {
        port->BSRR = (uint32_t)pin; // set pin high using bsrr register
    }
}

/**
 * @brief Resets a GPIO pin low using the BSRR register.
 * @param port Pointer to GPIO port structure.
 * @param pin Pin mask to set low.
 */
static inline void GPIO_ResetPin(GPIO_TypeDef *const port, uint16_t pin)
{
    if (NULL != port) // validate gpio port pointer
    {
        port->BSRR = (uint32_t)pin << 16U; // set pin low using bsrr register
    }
}

/**
 * @brief Toggles the state of a GPIO pin.
 * @param port Pointer to GPIO port structure.
 * @param pin Pin mask to toggle.
 */
static inline void GPIO_TogglePin(GPIO_TypeDef *const port, uint16_t pin)
{
    if (NULL != port) // validate gpio port pointer
    {
        port->ODR ^= (uint32_t)pin; // toggle pin state in output data register
    }
}

/**
 * @brief Transmits a single byte over SPI1 via direct register access with timeout.
 * @param data Byte to transmit over SPI bus.
 */
static inline void SPI1_WriteByte(uint8_t data)
{
    uint32_t timeout = 10000U; // timeout loop counter

    SPI1->CR1 |= SPI_CR1_SPE; // make sure spi peripheral is enabled

    while ((0U == (SPI1->SR & SPI_SR_TXE)) && (timeout > 0U)) // wait for tx buffer empty or timeout
    {
        timeout--; // decrement timeout counter
    }

    *(volatile uint8_t *)&SPI1->DR = data; // write byte directly to 8-bit data register

    timeout = 10000U; // reset timeout loop counter

    while ((0U != (SPI1->SR & SPI_SR_BSY)) && (timeout > 0U)) // wait until spi transfer completes or timeout
    {
        timeout--; // decrement timeout counter
    }
}

/**
 * @brief Sends a dummy byte and receives a single byte from SPI1 with timeout.
 * @retval uint8_t Received byte from SPI data register.
 */
static inline uint8_t SPI1_ReadByte(void)
{
    uint8_t rx_data = 0U;      // initialize return value byte
    uint32_t timeout = 10000U; // timeout loop counter

    SPI1->CR1 |= SPI_CR1_SPE; // make sure spi peripheral is enabled

    while ((0U == (SPI1->SR & SPI_SR_TXE)) && (timeout > 0U)) // wait for tx buffer empty or timeout
    {
        timeout--; // decrement timeout counter
    }

    *(volatile uint8_t *)&SPI1->DR = 0x00U; // send clock dummy byte to initiate read

    timeout = 10000U; // reset timeout loop counter

    while ((0U == (SPI1->SR & SPI_SR_RXNE)) && (timeout > 0U)) // wait for rx buffer full or timeout
    {
        timeout--; // decrement timeout counter
    }

    rx_data = *(volatile uint8_t *)&SPI1->DR; // read received byte from data register

    return rx_data; // return received byte via single exit point
}

/**
 * @brief Provides a cycle-accurate microsecond delay for 32 MHz CPU clock.
 * @param us Duration to delay in microseconds.
 */
static inline void delay_us(uint32_t us)
{
    uint32_t cycles = us * 8U; // calculate 8 clock cycles per microsecond at 32MHz

    while (0U != cycles) // loop until cycle counter reaches zero
    {
        __NOP();  // execute no operation instruction for single cycle delay
        cycles--; // decrement cycle count
    }
}

/**
 * @brief Converts an ASCII character to its corresponding Morse code string pattern.
 * @param c Input ASCII character to map.
 * @retval const char* Pointer to constant string containing dots, dashes, or space.
 */
const char *SX1276_GetMorsePattern(char c)
{
    const char *p_pattern = ""; // default to empty string for unknown characters
    char upper_c = c;           // local variable for character normalization

    if ((c >= 'a') && (c <= 'z')) // convert lowercase to uppercase
    {
        upper_c = (char)(c - ('a' - 'A')); // normalize ASCII offset
    }

    switch (upper_c) // match character to morse code pattern
    {
    case 'A':
    {
        p_pattern = ".-";
    }
    break;
    case 'B':
    {
        p_pattern = "-...";
    }
    break;
    case 'C':
    {
        p_pattern = "-.-.";
    }
    break;
    case 'D':
    {
        p_pattern = "-..";
    }
    break;
    case 'E':
    {
        p_pattern = ".";
    }
    break;
    case 'F':
    {
        p_pattern = "..-.";
    }
    break;
    case 'G':
    {
        p_pattern = "--.";
    }
    break;
    case 'H':
    {
        p_pattern = "....";
    }
    break;
    case 'I':
    {
        p_pattern = "..";
    }
    break;
    case 'J':
    {
        p_pattern = ".---";
    }
    break;
    case 'K':
    {
        p_pattern = "-.-";
    }
    break;
    case 'L':
    {
        p_pattern = ".-..";
    }
    break;
    case 'M':
    {
        p_pattern = "--";
    }
    break;
    case 'N':
    {
        p_pattern = "-.";
    }
    break;
    case 'O':
    {
        p_pattern = "---";
    }
    break;
    case 'P':
    {
        p_pattern = ".--.";
    }
    break;
    case 'Q':
    {
        p_pattern = "--.-";
    }
    break;
    case 'R':
    {
        p_pattern = ".-.";
    }
    break;
    case 'S':
    {
        p_pattern = "...";
    }
    break;
    case 'T':
    {
        p_pattern = "-";
    }
    break;
    case 'U':
    {
        p_pattern = "..-";
    }
    break;
    case 'V':
    {
        p_pattern = "...-";
    }
    break;
    case 'W':
    {
        p_pattern = ".--";
    }
    break;
    case 'X':
    {
        p_pattern = "-..-";
    }
    break;
    case 'Y':
    {
        p_pattern = "-.--";
    }
    break;
    case 'Z':
    {
        p_pattern = "--..";
    }
    break;

    case '0':
    {
        p_pattern = "-----";
    }
    break;
    case '1':
    {
        p_pattern = ".----";
    }
    break;
    case '2':
    {
        p_pattern = "..---";
    }
    break;
    case '3':
    {
        p_pattern = "...--";
    }
    break;
    case '4':
    {
        p_pattern = "....-";
    }
    break;
    case '5':
    {
        p_pattern = ".....";
    }
    break;
    case '6':
    {
        p_pattern = "-....";
    }
    break;
    case '7':
    {
        p_pattern = "--...";
    }
    break;
    case '8':
    {
        p_pattern = "---..";
    }
    break;
    case '9':
    {
        p_pattern = "----.";
    }
    break;

    case '/':
    {
        p_pattern = "-..-.";
    }
    break;
    case ' ':
    {
        p_pattern = " ";
    }
    break;

    default:
    {
        p_pattern = ""; // empty pattern for unmapped character
    }
    break;
    }

    return p_pattern; // single exit point
}

/**
 * @brief Transmits and receives a single byte over SPI1 via direct register access.
 * @param data Byte to transmit over SPI bus.
 * @retval uint8_t Byte received from SPI data register during transfer.
 */
static inline uint8_t SPI1_TransferByte(uint8_t data)
{
    uint8_t rx_data = 0U;      // initialize return value byte
    uint32_t timeout = 10000U; // timeout loop counter

    SPI1->CR1 |= SPI_CR1_SPE; // make sure spi peripheral is enabled

    rx_data = *(volatile uint8_t *)&SPI1->DR; // read DR to clear any leftover RXNE or OVR flag
    (void)rx_data;                            // suppress unused variable compiler warning

    while ((0U == (SPI1->SR & SPI_SR_TXE)) && (timeout > 0U)) // wait until transmit buffer is empty or timeout
    {
        timeout--; // decrement timeout counter
    }

    *(volatile uint8_t *)&SPI1->DR = data; // write byte directly to 8-bit data register

    timeout = 10000U; // reset timeout loop counter

    while ((0U == (SPI1->SR & SPI_SR_RXNE)) && (timeout > 0U)) // wait until receive buffer is full or timeout
    {
        timeout--; // decrement timeout counter
    }

    rx_data = *(volatile uint8_t *)&SPI1->DR; // read received byte from data register

    return rx_data; // return received byte via single exit point
}

sx1276_status_t SX1276_WriteRegister(sx1276_handle_t *const p_dev, uint8_t reg, uint8_t value)
{
    GPIO_ResetPin(p_dev->cs_port, p_dev->cs_pin); // drive cs low to start spi frame

    (void)SPI1_TransferByte(reg | 0x80U); // send register address with write bit set
    (void)SPI1_TransferByte(value);       // send register value byte

    GPIO_SetPin(p_dev->cs_port, p_dev->cs_pin); // drive cs high to end spi frame
    delay_us(2U);                               // wait for cs high relaxation time (t_NSS,HIGH)

    return SX1276_STATUS_OK; // single exit point
}

/**
 * @brief Reads a byte value from an SX1276 register over SPI.
 * @param p_dev Pointer to SX1276 device handle structure.
 * @param reg Register address to read.
 * @param p_value Pointer to output variable where read byte will be stored.
 * @retval sx1276_status_t Returns STATUS_OK on success or STATUS_ERROR on failure.
 */
sx1276_status_t SX1276_ReadRegister(sx1276_handle_t *const p_dev, uint8_t reg, uint8_t *const p_value)
{
    HAL_GPIO_WritePin(p_dev->cs_port, p_dev->cs_pin, GPIO_PIN_RESET); // drive cs low to start spi frame

    (void)SPI1_TransferByte(reg & 0x7FU); // send register address with read bit clear (bit 7 = 0)
    *p_value = SPI1_TransferByte(0x00U);  // send clock dummy byte and receive register payload

    HAL_GPIO_WritePin(p_dev->cs_port, p_dev->cs_pin, GPIO_PIN_SET); // drive cs high to end spi frame
    delay_us(2U);                                                   // wait for cs relaxation time (t_NSS,HIGH)

    return SX1276_STATUS_OK; // single exit point
}

/**
 * @brief Writes a byte to a register and immediately reads it back to verify the write operation.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param reg Target register address byte.
 * @param value Byte value expected to be written and verified.
 * @retval sx1276_status_t Status of the execution (OK, PARAM_ERR, SPI_ERR, or ERROR if readback mismatch).
 */
sx1276_status_t SX1276_WriteAndConfirm(sx1276_handle_t *const p_dev, uint8_t reg, uint8_t value)
{
    sx1276_status_t status = SX1276_STATUS_OK; // default return status to error
    uint8_t readback_val = 0U;                 // variable to store readback confirmation byte

    if (NULL != p_dev) // validate device handle pointer
    {
        status = SX1276_WriteRegister(p_dev, reg, value); // write target value to register

        if (SX1276_STATUS_OK == status) // proceed if write operation succeeded
        {
            HAL_Delay(60U); // allow sx1276 to start up if chaning from sleep to tx

            status = SX1276_ReadRegister(p_dev, reg, &readback_val); // read back register contents for confirmation

            if (SX1276_STATUS_OK == status) // verify readback was successful
            {
                if (readback_val != value) // check if readback matches expected value
                {
                    status = SX1276_STATUS_ERROR; // set error status on mismatch
                }
            }
        }
    }

    return status; // single exit point
}

/**
 * @brief Modifies specific bits in an SX1276 register using a mask and verifies the write.
 * @param p_dev Pointer to SX1276 device handle structure.
 * @param reg Register address to modify.
 * @param mask Bitmask indicating which bits to modify (1 = modify, 0 = preserve).
 * @param value New bit values to insert into masked position.
 * @retval sx1276_status_t Returns STATUS_OK on successful write and verification, else STATUS_ERROR.
 */
sx1276_status_t SX1276_ReadModifyWriteConfirm(sx1276_handle_t *const p_dev, uint8_t reg, uint8_t mask, uint8_t value)
{
    sx1276_status_t status = SX1276_STATUS_OK; // default return status to ok
    uint8_t current_val = 0U;                  // variable to store current register byte
    uint8_t target_val = 0U;                   // variable to store calculated target byte

    status = SX1276_ReadRegister(p_dev, reg, &current_val); // read current register contents

    if (SX1276_STATUS_OK == status) // proceed if read was successful
    {
        target_val = (current_val & (uint8_t)(~mask)); // clear bits specified by mask
        target_val |= (uint8_t)(value & mask);         // insert new bit values within mask

        status = SX1276_WriteAndConfirm(p_dev, reg, target_val); // write target value and verify register update
    }

    return status; // single exit point
}

/**
 * @brief Writes a sequence of bytes to a starting register (e.g., FIFO) in burst mode while holding CS low.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param reg Starting target register address byte.
 * @param p_tx_buffer Pointer to the data array containing bytes to send.
 * @param length Number of bytes to transmit.
 * @retval sx1276_status_t Status of the execution (OK, PARAM_ERR, or SPI_ERR).
 */
sx1276_status_t SX1276_WriteBuffer(sx1276_handle_t *const p_dev, const uint8_t reg, const uint8_t *const p_tx_buffer, const uint16_t length)
{
    sx1276_status_t status = SX1276_STATUS_OK; // status tracking variable

    if ((NULL == p_dev) || (NULL == p_dev->hspi) || (NULL == p_dev->cs_port) || (NULL == p_tx_buffer) || (0U == length)) // check for null pointers or zero length
    {
        status = SX1276_STATUS_ERR_PARAM; // set param error if bad input
    }
    else
    {
        uint8_t cmd = reg | 0x80U; // set bit 7 high for write command

        HAL_GPIO_WritePin(p_dev->cs_port, p_dev->cs_pin, GPIO_PIN_RESET); // pull CS low

        if (HAL_OK != HAL_SPI_Transmit(p_dev->hspi, &cmd, 1U, p_dev->spi_timeout_ms)) // send register write command byte
        {
            status = SX1276_STATUS_ERR_SPI; // flag spi error if command send failed
        }
        else
        {
            if (HAL_OK != HAL_SPI_Transmit(p_dev->hspi, (uint8_t *)p_tx_buffer, length, p_dev->spi_timeout_ms)) // stream out the data buffer
            {
                status = SX1276_STATUS_ERR_SPI; // flag spi error if payload send failed
            }
        }

        HAL_GPIO_WritePin(p_dev->cs_port, p_dev->cs_pin, GPIO_PIN_SET); // pull CS back high
    }

    return status; // return status
}

/**
 * @brief Reads a sequence of bytes from a starting register in burst mode into a destination buffer while holding CS low.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param reg Starting target register address byte.
 * @param p_rx_buffer Pointer to the destination memory buffer where incoming bytes will be stored.
 * @param length Number of bytes to read.
 * @retval sx1276_status_t Status of the execution (OK, PARAM_ERR, or SPI_ERR).
 */
sx1276_status_t SX1276_ReadBuffer(sx1276_handle_t *const p_dev, const uint8_t reg, uint8_t *const p_rx_buffer, const uint16_t length)
{
    sx1276_status_t status = SX1276_STATUS_OK; // status tracking variable

    if ((NULL == p_dev) || (NULL == p_dev->hspi) || (NULL == p_dev->cs_port) || (NULL == p_rx_buffer) || (0U == length)) // validate pointers and length
    {
        status = SX1276_STATUS_ERR_PARAM; // set param error if input invalid
    }
    else
    {
        uint8_t cmd = reg & 0x7FU; // set bit 7 low for read command

        HAL_GPIO_WritePin(p_dev->cs_port, p_dev->cs_pin, GPIO_PIN_RESET); // pull CS low

        if (HAL_OK != HAL_SPI_Transmit(p_dev->hspi, &cmd, 1U, p_dev->spi_timeout_ms)) // send register read command byte
        {
            status = SX1276_STATUS_ERR_SPI; // flag spi error if command send failed
        }
        else
        {
            if (HAL_OK != HAL_SPI_Receive(p_dev->hspi, p_rx_buffer, length, p_dev->spi_timeout_ms)) // read payload directly into rx buffer
            {
                status = SX1276_STATUS_ERR_SPI; // flag spi error if payload read failed
            }
        }

        HAL_GPIO_WritePin(p_dev->cs_port, p_dev->cs_pin, GPIO_PIN_SET); // pull CS back high
    }

    return status; // return status
}

/**
 * @brief Resets and initializes the SX1276 transceiver into 140 MHz beacon operational mode.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @retval sx1276_status_t Status of the initialization sequence.
 */
sx1276_status_t SX1276_Init(sx1276_handle_t *const p_dev)
{
    sx1276_status_t status = SX1276_STATUS_OK; // status variable for init sequence

    if ((NULL == p_dev) || (NULL == p_dev->hspi) || (NULL == p_dev->cs_port) || (NULL == p_dev->reset_port)) // check handle inputs
    {
        status = SX1276_STATUS_ERR_PARAM; // return param error if handles missing
    }
    else
    {
        HAL_GPIO_WritePin(p_dev->reset_port, p_dev->reset_pin, GPIO_PIN_RESET); // drive reset pin low
        HAL_Delay(2U);                                                          // wait 2ms for reset
        HAL_GPIO_WritePin(p_dev->reset_port, p_dev->reset_pin, GPIO_PIN_SET);   // release reset pin high
        HAL_Delay(10U);                                                         // wait 10ms for chip startup

        uint8_t version = 0U;                                 // variable to hold chip version
        status = SX1276_ReadRegister(p_dev, 0x42U, &version); // read silicon version register
        if (status == SX1276_STATUS_OK)                       // check read success
        {
            if (version != 0x12U) // check if version matches sx1276 (0x12)
            {
                status = SX1276_STATUS_ERROR; // set error if version mismatch
            }
            else
            {
                uint32_t frf = (uint32_t)(((uint64_t)BEACON_FREQUENCY_HZ * 524288ULL) / 32000000ULL); // calculate frequency register value
                uint8_t msb = (uint8_t)((frf >> 16) & 0xFFU);                                         // extract frequency msb byte
                uint8_t mid = (uint8_t)((frf >> 8) & 0xFFU);                                          // extract frequency mid byte
                uint8_t lsb = (uint8_t)(frf & 0xFFU);                                                 // extract frequency lsb byte

                status = SX1276_ReadModifyWriteConfirm(p_dev, 0x01U, 0x07U, 0x00U); // set FSK Sleep Mode
                if (status == SX1276_STATUS_OK)                                     // check status
                {
                    status = SX1276_WriteAndConfirm(p_dev, 0x09U, 0x00U); // set Low Power RFO mode, also the power output is here
                    if (status == SX1276_STATUS_OK)                       // check status
                    {
                        status = SX1276_WriteAndConfirm(p_dev, 0x06U, msb); // set frequency msb
                        if (status == SX1276_STATUS_OK)                     // check status
                        {
                            status = SX1276_WriteAndConfirm(p_dev, 0x07U, mid); // set frequency mid
                            if (status == SX1276_STATUS_OK)                     // check status
                            {
                                status = SX1276_WriteAndConfirm(p_dev, 0x08U, lsb); // set frequency lsb
                                if (status == SX1276_STATUS_OK)                     // check status
                                {
                                    status = SX1276_WriteAndConfirm(p_dev, 0x31U, 0x00U); // set continuous mode
                                    if (status == SX1276_STATUS_OK)                       // check status
                                    {
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return status; // return status
}

/**
 * @brief Transmits a Morse code string message over FM using the configured tone frequency and timing.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param p_message Pointer to the null-terminated string message to transmit.
 * @retval sx1276_status_t Status of execution.
 */
sx1276_status_t SX1276_TransmitMorse(sx1276_handle_t *const p_dev, const char *const p_message)
{
    sx1276_status_t status = SX1276_STATUS_OK; // status tracking variable

    if ((NULL == p_dev) || (NULL == p_dev->hspi) || (NULL == p_dev->cs_port) || (NULL == p_dev->reset_port) || (NULL == p_message)) // check handle and message inputs
    {
        status = SX1276_STATUS_ERR_PARAM; // return param error if handles missing
    }
    else
    {
        status = SX1276_ReadModifyWriteConfirm(p_dev, 0x01U, 0x07U, 0x03U); // wake up and set transmit mode (RegOpMode = 0x03)
        if (status == SX1276_STATUS_OK)                                     // check wake up success
        {
            uint16_t char_idx = 0U; // index counter for message characters

            while ('\0' != p_message[char_idx]) // loop through message characters until null terminator
            {
                const char *p_pattern = SX1276_GetMorsePattern(p_message[char_idx]); // get morse dot/dash pattern string for current character

                if (NULL != p_pattern) // check if pattern exists
                {
                    if (' ' == p_pattern[0]) // check if space character
                    {
                        HAL_Delay(MORSE_WORD_SPACE_MS); // delay for word space duration
                    }
                    else
                    {
                        uint16_t elem_idx = 0U; // index counter for pattern elements

                        while ('\0' != p_pattern[elem_idx]) // loop through dots and dashes in pattern
                        {
                            if ('.' == p_pattern[elem_idx]) // check if dot element
                            {
                                SX1276_PlayTone(p_dev, MORSE_DOT_MS); // play dot tone
                            }
                            else if ('-' == p_pattern[elem_idx]) // check if dash element
                            {
                                SX1276_PlayTone(p_dev, MORSE_DASH_MS); // play dash tone
                            }

                            HAL_Delay(MORSE_ELEM_SPACE_MS); // delay for element space between dots/dashes
                            elem_idx++;                     // advance element index
                        }

                        HAL_Delay(MORSE_LETTER_SPACE_MS); // delay for letter space between characters
                    }
                }

                char_idx++; // advance character index
            }

            status = SX1276_ReadModifyWriteConfirm(p_dev, 0x01U, 0x07U, 0x000); // return to FSK sleep mode when done
        }
    }

    return status; // return status
}

/**
 * @brief Generates an audio tone on the DIO2 pin at the frequency defined in the device handle for a given duration.
 * @param p_dev Pointer to the SX1276 device handle structure.
 * @param duration_ms Duration to play the audio tone in milliseconds.
 */
void SX1276_PlayTone(sx1276_handle_t *const p_dev, uint16_t duration_ms)
{
    if ((NULL != p_dev) && (NULL != p_dev->dio2_port) && (0U != duration_ms)) // validate device handle dio2 port and non-zero duration
    {
        uint32_t half_period_us = 1000000UL / (2UL * (uint32_t)BEACON_TONE_FREQ_HZ); // calculate tone half-period duration in microseconds
        uint32_t total_toggles = ((uint32_t)duration_ms * 1000UL) / half_period_us;  // calculate total dio2 pin toggles needed for duration

        while (0U != total_toggles) // loop until all waveform pin toggles finish
        {
            HAL_GPIO_TogglePin(p_dev->dio2_port, p_dev->dio2_pin); // toggle dio2 pin to generate audio tone wave
            delay_us(half_period_us);                              // delay for exact tone half-period in microseconds
            total_toggles--;                                       // decrement toggle count
        }

        GPIO_ResetPin(p_dev->dio2_port, p_dev->dio2_pin); // make sure dio2 pin ends in low state
    }
}