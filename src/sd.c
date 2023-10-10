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
  uint8_t r1 = 0;

  SSD1306_Init ();                                                // init lcd
  SSD1306_ClearScreen ();
  SSD1306_SetPosition (10, 0);
  SSD1306_DrawString ("SDCARD INTERFACING", NORMAL);
     
  SPI_PortInit ();                                      // SPI ports init
  SPI_SlowSpeedInit ();                                 // SPI slow speed 250kHz
  SD_PowerUp ();                                        // Power up sequence
  //uint8_t r1 = SD_IdleState ();                         // Idle state

  return r1;
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
  uint8_t i = 10;

  // Power Up Time Delay
  // ----------------------------------------------------------------
  //_delay_ms(250);                                       // power up time

  // Supply Ram Up Sequence
  // ----------------------------------------------------------------
  CS_DISABLE();                                         // hold CS high
  _delay_ms(1);                                         // supply ramp up time
  
  SSD1306_SetPosition (0, 2);
  
  while (i) {                                           // supply ramp up cycles
    SPI_WriteByte (0xff);                               // min 74 cycles
    SSD1306_DrawChar ('A', NORMAL);
    i--;
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
  SPI_WriteByte (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_WriteByte (0xff);

  SD_SendCommand (SD_CMD0, SD_CMD0_ARG, SD_CMD0_CRC);   // Send CMD0
  uint8_t response = SD_GetResponseR1 ();               // response

  SPI_WriteByte (0xff);
  CS_DISABLE ();                                        // CS High
  SPI_WriteByte (0xff);

  return response;
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
  SPI_WriteByte (cmd);                                  // send command
  SPI_WriteByte ((uint8_t) (arg >> 24));                // send arguments
  SPI_WriteByte ((uint8_t) (arg >> 16));                //
  SPI_WriteByte ((uint8_t) (arg >>  8));                //
  SPI_WriteByte ((uint8_t) (arg >>  0));                //
  SPI_WriteByte (crc | 0x01);                           // cyclic redundancy check
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
  uint8_t response = 0xff;

  while ((response = SPI_ReadByte ()) == 0xff) {
    if (++i > 8) {
      break;
    }
  }

  return response;
}
