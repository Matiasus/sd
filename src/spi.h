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

  // INCLUDE libraries
  #include <avr/io.h>

  // atmega328p
  #if defined(__AVR_ATmega328P__)

    #define SPI_DDR           DDRB
    #define SPI_PORT          PORTB
    #define SPI_SCK           5     // or SCLK
    #define SPI_MISO          4     // or DO
    #define SPI_MOSI          3     // or SDA, SDI, DI
    #define SPI_SS            2     // or CS

    // SPI registers
    #define SPI_SPSR          SPSR
    #define SPI_SPCR          SPCR
    #define SPI_SPDR          SPDR

  #endif

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
   * @desc    SPI Fast Speed Init
   *
   * @param   void
   *
   * @return  void
   */
  void SPI_FastSpeedInit (void);

  /**
   * @desc    SPI Write Byte
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void SPI_WriteByte (uint8_t);

  /**
   * @desc    SPI Write Word / big endian (MSB first)
   *
   * @param   uint16_t
   *
   * @return  void
   */
  void SPI_WriteWord (uint16_t);

  /**
   * @desc    SPI Read Byte
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SPI_ReadByte (void);

  /**
   * @desc    SPI Write / Read Byte
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
   uint8_t SPI_WriteReadByte (uint8_t);

#endif
