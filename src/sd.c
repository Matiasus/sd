/**
 * --------------------------------------------------------------------------------------+
 * @brief       SD / uSD CARD 
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
#include <delay.h>
#include "sd.h"

/**
 * @brief   SD Card Power Up Sequence
 *
 * @param   void
 *
 * @return  void
 */
void SD_PowerUp (void)
{
  uint8_t i = 10;
  
  // Power Up Time Delay
  // ----------------------------------------------------------------
  _delay_ms(250);                                       // power up time

  // Supply Ram Up Sequence
  // ----------------------------------------------------------------
  CS_DISABLE();                                         // hold CS high
  _delay_ms(1);                                         // supply ramp up time 
  while (i--) {                                         // supply ramp up cycles
    SPI_WriteByte (0xff);                               // min 74 cycles
  }
  // Deselect Card
  // accor. http://www.rjhcoding.com/avrc-sd-interface-1.php
  // ----------------------------------------------------------------
  CS_DISABLE();
  SPI_WriteByte (0xff);
}

/**
 * @brief   SD Card Power Up Sequence
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SD_IdleState (void)
{
  uint8_t response = 0xff;
  
  CS_ENABLE ();                                         // CS low
  SD_SendCommand (SD_CMD0, SD_CMD0_ARG, SD_CMD0_CRC);   // Send CMD0
  if (SD_GetResponseR1 != 0x01) {
    return SD_GetResponseR1;
  }  
  CS_DISABLE ();                                        // CS High
  
  return response;
}

/**
 * @brief   SD Card Init
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SD_Init (void)
{
  SPI_PortInit ();                                      // SPI ports init
  SPI_SlowSpeedInit ();                                 // SPI slow speed 250kHz
  SD_PowerUp ();                                        // Power up sequence
  
  return 0;
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
  SPI_WriteByte (command);                              // send command
  SPI_WriteByte ((uint8_t) argument >> 24);             // send arguments
  SPI_WriteByte ((uint8_t) argument >> 16);             // 
  SPI_WriteByte ((uint8_t) argument >>  8);             // 
  SPI_WriteByte ((uint8_t) argument >>  0);             // 
  SPI_WriteByte (crc | 0x01);                           // cyclic redundancy check
}

/**
 * @brief   SD Card Response R1
 *
 * @param   void
 *
 * @return  uint8_t
 */
void SD_GetResponseR1 (void)
{
  uint8_t response = 0xff;
  
  response = SPI_ReadByte ();
  
  return response;
}
