/*
 * ============================================================================
 * Project      : Beacon
 * File         : main.h
 * Author       : Goran Staznik
 * Organization : Staznik Systems
 * Created      : 2026 - 10 - 22
 * Description  : Header file for main
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

#ifndef MAIN_H // include guard to prevent multiple inclusions
#define MAIN_H // define main header guard macro

#include <stdint.h>  // standard fixed-width integer types
#include <stdbool.h> // standard boolean definitions
#include <stddef.h>  // standard size and NULL definitions

#include "bsp_beacon.h"    // board support package header for hardware setup
#include "sx1276_driver.h" // sx1276 radio driver header

/* USER CONFIGURATION PARAMETERS */
#define BEACON_FREQUENCY_HZ (144200000UL) // target rf frequency in Hz (144.2 MHz RAC 2m simplex)
#define BEACON_CALLSIGN "CALLSIGN"        // callsign or ID string to transmit in morse code
#define BEACON_MESSAGE "ROCKET BKN"       // message to follow the callsign every interval minutes (suggest add "No RX")
#define BEACON_ID_INTERVAL_MIN (10U)      // time between every identification in minutes (NO MORE THAN 30 in Canada)
#define BEACON_INTERVAL_SEC (10U)         // wait time between beacon transmissions in seconds
#define BEACON_MORSE_WPM (15U)            // morse code speed in words per minute
#define BEACON_TONE_FREQ_HZ (392U)        // audio pitch tone frequency in Hz

#endif /* MAIN_H */ // end of include guard
