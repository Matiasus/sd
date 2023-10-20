/**
 * ---------------------------------------------------------------+
 * @desc        SPI (Master Mode)
 * ---------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       21.09.2022
 * @file        spi.h
 * @version     1.0
 * @tested      AVR Atmega328p
 *
 * @depend      avr/io.h
 * ---------------------------------------------------------------+
 * @interface   SPI master mode
 * @pins        SCLK, MOSI, MISO, CS (SS)
 *
 * @sources
 */

#ifndef __SPI_H__
#define __SPI_H__

  // includes
  #include <avr/io.h>

  // atmega328p
  #if defined(__AVR_ATmega328P__)

    #define SPI_DDR           DDRB
    #define SPI_PORT          PORTB
    #define SPI_SCK           PINB5     // or SCLK
    #define SPI_MISO          PINB4     // or DO
    #define SPI_MOSI          PINB3     // or SDA, SDI, DI
    #define SPI_SS            PINB2     // or CS

    // SPI registers
    #define SPI_SPSR          SPSR
    #define SPI_SPCR          SPCR
    #define SPI_SPDR          SPDR

  #endif

  // macros
  #define CS_ENABLE()         SPI_PORT &= ~(1 << SPI_SS)
  #define CS_DISABLE()        SPI_PORT |= (1 << SPI_SS)

  /**
   * @desc    SPI Port Init
   *
   * @param   void
   *
   * @return  void
   */
  void SPI_PortInit (void);

  /**
   * @desc    SPI Slow Speed Init
   *
   * @param   void
   *
   * @return  void
   */
  void SPI_SlowSpeedInit (void);

  /**
   * @desc    SPI Write Byte
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SPI_Transfer (uint8_t);
  
#endif
