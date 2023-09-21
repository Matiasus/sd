/**
 * --------------------------------------------------------------------------------------+
 * @brief       SD CARD INTERFACING
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2023 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        20.09.2023
 * @file        sd.h
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend      spi.h
 * --------------------------------------------------------------------------------------+
 * @interface   SPI
 * @pins        
 *
 * @sources     
 */

#ifndef __SD_H__
#define __SD_H__

  // INCLUDE libraries
  // ------------------------------------------------------------------
  #include <avr/io.h>
  #include "spi.h"

  // PORT / PIN
  // ------------------------------------------------------------------
  #define SD_DDR              SPI_DDR
  #define SD_PORT             SPI_PORT

  #define SD_SCK              SPI_SCK
  #define SD_MOSI             SPI_MOSI
  #define SD_MISO             SPI_MISO
  #define SD_XCS              SPI_SS
  #define SD_DREQ             1
  #define SD_XDCS             0

  // SD CARD COMMAND TABLE
  // ------------------------------------------------------------------
  #define SD_CMD0             (0x40+0)  // GO_IDLE_STATE / Reset the SD Memory Card
  #define SD_CMD1             (0x40+1)  // SD_SEND_OP_COND (MMC) / activate the card's initialization process
  #define SD_ACMD41           (0xC0+41) // SD_SEND_OP_COND (SDC)
  #define SD_CMD8             (0x40+8)  // SEND_IF_COND / voltage information and asks the accessed card whether card can operate in supplied voltage range
  #define SD_CMD9             (0x40+9)  // SEND_CSD / asks the selected card to send its card-specific data (CSD)
  #define SD_CMD10            (0x40+10) // SEND_CID / asks the selected card to send its card identification (CID)
  #define SD_CMD12            (0x40+12) // STOP_TRANSMISSION / force the card to stop transmission in Mutliple Block Read Operation
  #define SD_ACMD13           (0xC0+13) // SD_STATUS (SDC) / asks the selected card to send its status register
  #define SD_CMD16            (0x40+16) // SET_BLOCKLEN
  #define SD_CMD17            (0x40+17) // READ_SINGLE_BLOCK
  #define SD_CMD18            (0x40+18) // READ_MULTIPLE_BLOCK
  #define SD_CMD23            (0x40+23) // SET_BLOCK_COUNT (MMC)
  #define SD_ACMD23           (0xC0+23) // SET_WR_BLK_ERASE_COUNT (SDC)
  #define SD_CMD24            (0x40+24) // WRITE_BLOCK
  #define SD_CMD25            (0x40+25) // WRITE_MULTIPLE_BLOCK
  #define SD_CMD55            (0x40+55) // APP_CMD
  #define SD_CMD58            (0x40+58) // READ_OCR
  #define SD_CMD59            (0x40+59) // CRC_ON_OFF
  
  /**
   * @brief   SD Card Init
   *
   * @param   void
   *
   * @return  void
   */
  void SD_Init (void);
  
  /**
   * @brief   SD Card Send Command
   *
   * @param   uint8_t
   * @param   uint32_t
   * @param   uint8_t
   *
   * @return  void
   */
  void SD_SendCommand (uint8_t command, uint32_t argument, uint8_t crc);

#endif
