#ifndef MAIN_H // include guard to prevent multiple inclusions
#define MAIN_H // define main header guard macro

#include <stdint.h>  // standard fixed-width integer types
#include <stdbool.h> // standard boolean definitions
#include <stddef.h>  // standard size and NULL definitions

#include "bsp_beacon.h"    // board support package header for hardware setup
#include "sx1276_driver.h" // sx1276 radio driver header

/* Beacon Configuration Parameters */
#define BEACON_FREQUENCY_HZ (144200000UL) // target rf frequency in Hz (144.2 MHz RAC 2m simplex)
#define BEACON_CALLSIGN "CALLSIGN"        // callsign or ID string to transmit in morse code
#define BEACON_MESSAGE "ROCKET BKN"       // message to follow the callsign every interval minutes (suggest add "No RX")
#define BEACON_ID_INTERVAL_MIN (10U)      // time between every identification in minutes (NO MORE THAN 30 in Canada)
#define BEACON_INTERVAL_SEC (10U)         // wait time between beacon transmissions in seconds
#define BEACON_MORSE_WPM (15U)            // morse code speed in words per minute
#define BEACON_TONE_FREQ_HZ (392U)        // audio pitch tone frequency in Hz

#endif /* MAIN_H */ // end of include guard
