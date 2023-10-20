/**
 * ---------------------------------------------------------------+
 * @desc        SPI (Master Mode)
 * ---------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       21.09.2023
 * @file        spi.c
 * @version     1.0
 * @tested      AVR Atmega328p
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
 * @desc    SPI Port Init
 *
 * @param   void
 *
 * @return  void
 */
void SPI_PortInit (void)
{
  SPI_DDR |= (1 << SPI_SS) | (1 << SPI_MOSI) | (1 << SPI_SCK);
  SPI_DDR &= ~(1 << SPI_MISO);
  SPI_PORT |= (1 << SPI_MISO);
}

/**
 * @desc    SPI Slow Speed Init
 *
 * @param   void
 *
 * @return  void
 */
void SPI_SlowSpeedInit (void)
{
  SPI_SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
  SPI_SPSR &= ~(1 << SPI2X);
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
