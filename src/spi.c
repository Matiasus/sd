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
  SPI_DDR |= (1 << SPI_SS)  |               // outputs
             (1 << SPI_SCK) |               //
             (1 << SPI_MOSI);               //
  SPI_DDR &= ~(1 << SPI_MISO);              // inputs
  SPI_PORT |= (1 << SPI_MISO);              // pullup activate
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
  SPI_SPCR = (1 << SPE)  |                  // SPI Enable, note: writing a byte to the SPI data reg starts the SPI clock generator
             (1 << MSTR) |                  // Master device
             (1 << SPR1);                   // Prescaler fclk/64 = 500 000 Hz
  SPI_SPSR = (1 << SPI2X);                  // f*2             
}

/**
 * @desc    SPI Fast Speed Init
 *
 * @param   void
 *
 * @return  void
 */
void SPI_FastSpeedInit (void)
{
  SPI_SPCR = (1 << SPE)  |                  // SPI Enable, note: writing a byte to the SPI data reg starts the SPI clock generator
             (1 << MSTR) |                  // Master device
             (1 << SPR0) ;                  // f = fclk/16 = 0.5MHz
  SPI_SPSR = (1 << SPI2X);                  // f*2
}

/**
 * @desc    SPI Write Byte
 *
 * @param   uint8_t
 *
 * @return  void
 */
void SPI_WriteByte (uint8_t data)
{
  SPI_SPDR = data;                          // start transmission
  while(!(SPI_SPSR & (1<<SPIF)))            // wait for transmission complete
  ;
}

/**
 * @desc    SPI Write Word / big endian (MSB Byte first)
 *
 * @param   uint16_t
 *
 * @return  void
 */
void SPI_WriteWord (uint16_t data)
{
  SPI_WriteByte ((uint8_t)(data >> 8));     // high byte
  SPI_WriteByte ((uint8_t)(data & 0xFF));   // low byte
}

/**
 * @desc    SPI Read Byte
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SPI_ReadByte (void)
{
  SPI_SPDR = 0xFF;                          // start transmission, dummy byte
  while(!(SPI_SPSR & (1<<SPIF)))            // wait for transmission complete
  ;
  return SPI_SPDR;                          // return received byte
}

/**
 * @desc    SPI Write / Read Byte
 *
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SPI_WriteReadByte (uint8_t data)
{
  SPI_SPDR = data;                          // start transmission
  while(!(SPI_SPSR & (1<<SPIF)))            // wait for transmission complete
  ;
  return SPI_SPDR;                          // return received byte
}
