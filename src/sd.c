/**
 * --------------------------------------------------------------------------------------+
 * @brief       SD CARD INTERFACING
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2023 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        20.09.2023
 * @file        sd.c
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend      sd.h
 * --------------------------------------------------------------------------------------+
 * @interface   SPI
 * @pins        
 *
 * @sources     
 */

// INCLUDE libraries
// ------------------------------------------------------------------
#include "sd.h"

const uint8_t SD_SEQUENCE[] PROGMEM = {
   x
  ,SD_CMD0,   0, 95
  ,SD_ACMD41, 0, xx
  ,SD_CMD8,   0, xx
}

/**
 * @brief   SD Card Init
 *
 * @param   void
 *
 * @return  void
 */
void SD_Init (void)
{
  SD_SendCommand (SD_CMD0, SD_CMD0_ARG0, CRC);
}

/**
 * @brief   SD Card Send Command
 *
 * @param   uint8_t
 * @param   uint32_t
 * @param   uint8_t
 *
 * @return  void
 */
void SD_SendCommand (uint8_t command, uint32_t argument, uint8_t crc)
{
  
}
