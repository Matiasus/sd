/**
 * ---------------------------------------------------------------+
 * @brief       SPI (Master Mode)
 * ---------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        21.09.2023
 * @file        spi.c
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend
 * ---------------------------------------------------------------+
 * @interface   SPI master mode
 * @pins        SCLK, MOSI, MISO, CS (SS)
 *
 * @sources
 */

// INCLUDE libraries
#include "spi.h"

/**
 * @desc    SPI Init
 *
 * @param   uint8_t settings
 * @param   uint8_t double speed
 * 
 * @return  void
 */
void SPI_Init (uint8_t settings, uint8_t double_speed)
{
  // SPI PORT Init
  // ----------------------------------------------------------------
  SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);
  SPI_DDR &= ~(1 << SPI_MISO);
  SPI_PORT |= (1 << SPI_MISO);

  // Doble Speed ?
  // ----------------------------------------------------------------
  (double_speed == 1) ? (SPI_SPSR |= (1 << SPI2X)) : (SPI_SPSR &= ~(1 << SPI2X));
  
  // SPI init
  // ----------------------------------------------------------------
  SPI_SPCR = settings;
}

/**
 * @desc    SPI Enable
 *
 * @param   void
 *
 * @return  void
 */
void SPI_Enable (void)
{
  SPI_SPCR |= (1 << SPE);
}

/**
 * @desc    SPI Send & Receive Byte
 *
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SPI_Transfer (uint8_t data)
{
  SPI_SPDR = data;
  while(!(SPI_SPSR & (1<<SPIF))) 
  ;
  return SPI_SPDR;
}