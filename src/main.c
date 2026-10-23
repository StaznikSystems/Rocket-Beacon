#include "main.h"
#include "bsp_beacon.h"
#include "sx1276_driver.h"

/**
 * @brief Overrides standard C library startup constructor initialization.
 */
void __libc_init_array(void)
{
  /* empty stub to bypass C++ constructor array and prevent RAM execution crash */
}

static sx1276_handle_t beacon_radio = {
    .hspi = &hspi1,                   // pointer to spi1 handle
    .cs_port = SPI1_CS_GPIO_Port,     // set chip select gpio port
    .cs_pin = SPI1_CS_Pin,            // set chip select gpio pin
    .reset_port = SX_NREST_GPIO_Port, // set radio reset gpio port
    .reset_pin = SX_NREST_Pin,        // set radio reset gpio pin
    .dio2_port = DIO2_GPIO_Port,      // set dio2 gpio port
    .dio2_pin = DIO2_Pin,             // set dio2 gpio pin
    .spi_timeout_ms = 100U            // set spi timeout in milliseconds
};

int main(void)
{

  BSP_Initialize(); // Initialize the BSP

  if (SX1276_STATUS_OK != SX1276_Init(&beacon_radio)) // Initialize the radio and check if it worked
  {
    while (1) // if it didn't work sit here forever and blink error led
    {
      HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_Pin); // toggle led
      HAL_Delay(1000U);                           // wait 100ms
    }
  }

  HAL_Delay(500U); // wait 0.5 seconds before ready flash

  /* Flash the LED twice to show all good */
  BSP_FLashLED(2U);

  /* Main Loop */
  while (1)
  {
    const uint32_t numDash = ((uint32_t)BEACON_ID_INTERVAL_MIN * 60U) / (uint32_t)BEACON_INTERVAL_SEC; // calculate total dash repetitions per id cycle
    uint32_t numTx = 0U;                                                                               // counter to count how many times the standard transmission has been sent out

    if (SX1276_STATUS_OK != SX1276_TransmitMorse(&beacon_radio, BEACON_CALLSIGN)) // Send out the callsign in Morse
    {                                                                             // If that didn't work flash a bit to let the user know
      BSP_FLashLED(3U);
    }

    HAL_Delay(MORSE_WORD_SPACE_MS); // Delay after the callsign so they are different

    if (SX1276_STATUS_OK != SX1276_TransmitMorse(&beacon_radio, BEACON_MESSAGE)) // Send out the message in Morse
    {
      BSP_FLashLED(3U);
    }

    HAL_Delay(MORSE_WORD_SPACE_MS); // Delay after the callsign so they are different

    while (numTx < numDash) // while numTx is less than number of transmissions in the intervals
    {
      if (SX1276_STATUS_OK != SX1276_TransmitMorse(&beacon_radio, "T")) // Send out "T" (-) in Morse
      {
        BSP_FLashLED(3U);
      }
      numTx++;                                     // increment numTx
      LowPower_Delay(BEACON_INTERVAL_SEC * 1000U); // sleep MCU via BSP low power delay until next beacon transmission
    }
  }
}