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
 * @pins        MOSI, MISO, CLK, SS, UCC, USS
 *
 * @sources     http://www.rjhcoding.com/avrc-sd-interface-1.php
 */

// INCLUDE libraries
// ------------------------------------------------------------------
#include "sd.h"

/**
 * @brief   SD Card Init
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SD_Init (void)
{
  uint8_t r1;
  uint8_t i = 0;
  uint8_t r[5];

  SD_PowerUp ();

  while ((r1 = SD_IdleState ()) != SD_IDLE_STATE) {
    if (++i > SD_CMD0_MAX_CYCLE) {
      return SD_ERROR;
    }
  }
  
  _delay_ms(1);

  if (SD_SendIfCond (r) == SD_ERROR) {
    return SD_ERROR;
  }


  return SD_SUCCESS;
}

/**
 * @brief   SD Card Power Up Sequence
 *
 * @param   void
 *
 * @return  void
 */
void SD_PowerUp (void)
{
  // Power Up Time Delay
  // ----------------------------------------------------------------
  _delay_ms(250);                                       // power up time

  // Supply Ram Up Sequence
  // ----------------------------------------------------------------
  CS_DISABLE ();                                        // hold CS high
  _delay_ms (1);                                        // supply ramp up time

  for (uint8_t i=0; i<10; i++) {
    SPI_Transfer (0xff);
  }

  // Deselect Card
  // accor. http://www.rjhcoding.com/avrc-sd-interface-1.php
  // ----------------------------------------------------------------
  CS_DISABLE ();                                        // hold CS high
  SPI_Transfer (0xff);                                  // dummy byte
}

/**
 * @brief   SD Card Idle State
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SD_IdleState (void)
{
  SPI_Transfer (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_Transfer (0xff);

  SD_SendCommand (SD_CMD0, SD_CMD0_ARG, SD_CMD0_CRC);   // Send CMD0
  uint8_t response = SD_GetResponseR1 ();               // response

  SPI_Transfer (0xff);
  CS_DISABLE ();                                        // CS High
  SPI_Transfer (0xff);

  return response;
}

/**
 * @brief   SD Card Send if condition
 *
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SD_SendIfCond (uint8_t * r)
{
  SPI_Transfer (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_Transfer (0xff);

  SD_SendCommand (SD_CMD8, SD_CMD8_ARG, SD_CMD8_CRC);   // Send CMD0
  uint8_t response = SD_GetResponseR7 (r);              // response

  SPI_Transfer (0xff);
  CS_DISABLE ();                                        // CS High
  SPI_Transfer (0xff);
  
  return response;
}

/**
 * @brief   SD Card Response R1
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SD_GetResponseR1 (void)
{
  uint8_t i = 0;
  uint8_t response;

  while ((response = SPI_Transfer (0xff)) == 0xff) {
    if (++i > 8) {
      break;
    }
  }

  return response;
}

/**
 * @brief   SD Card Response R7
 *
 * @param   uint8_t *
 *
 * @return  uint8_t
 */
uint8_t SD_GetResponseR7 (uint8_t * r)
{
  if ((r[0] = SD_GetResponseR1 ()) > 1) {
    return SD_ERROR;
  }

  for (uint8_t i=1; i<5; i++) {
    r[i] = SD_GetResponseR1 ();
  }

  return SD_SUCCESS;
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
void SD_SendCommand (uint8_t cmd, uint32_t arg, uint8_t crc)
{
  SPI_Transfer (cmd | 0x40);                           // send command
  SPI_Transfer ((uint8_t) (arg >> 24));                // send arguments
  SPI_Transfer ((uint8_t) (arg >> 16));                //
  SPI_Transfer ((uint8_t) (arg >>  8));                //
  SPI_Transfer ((uint8_t) (arg >>  0));                //
  SPI_Transfer (crc | 0x01);                           // cyclic redundancy check
}
