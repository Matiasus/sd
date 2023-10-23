/**
 * --------------------------------------------------------------------------------------+
 * @brief       SD / uSD CARD
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
 * @depend      avr/io.h, spi.h
 * --------------------------------------------------------------------------------------+
 * @interface   SPI 4-wire
 * @pins
 *
 * @sources
 */

#ifndef __SD_H__
#define __SD_H__

  // INCLUDE libraries
  // ------------------------------------------------------------------
  #include <avr/io.h>
  #include <util/delay.h>
  #include "spi.h"
  #include "lcd/ssd1306.h"
  #include <stdio.h>

  // RETURN
  // ------------------------------------------------------------------
  #define SD_ERROR                0xff
  #define SD_SUCCESS              0x00

  // PORT / PIN
  // ------------------------------------------------------------------
  #define SD_DDR                  SPI_DDR
  #define SD_PORT                 SPI_PORT

  #define SD_CLK                  SPI_SCK     // CLK
  #define SD_CMD                  SPI_MOSI    // CMD
  #define SD_DAT0                 SPI_MISO    // DAT0
  #define SD_DAT3                 SPI_SS      // CS/DAT3

  // SD CARD COMMAND TABLE
  // ------------------------------------------------------------------
  #define SD_CMD0                 (0x40+0)    // GO_IDLE_STATE / Reset the SD Memory Card
  #define SD_CMD0_ARG             0x00000000
  #define SD_CMD0_CRC             0x95

  #define SD_CMD8                 (0x40+8)    // SEND_IF_COND / voltage information and asks the accessed card whether card can operate in supplied voltage range
  #define SD_CMD8_ARG             0x000001AA  // check pattern AA / Phzsical Layer Spec Version 3.01 page 62
  #define SD_CMD8_CRC             0x87

  #define SD_CMD58                (0x40+58)   // READ_OCR
  #define SD_CMD55                (0x40+55)   // APP_CMD
  #define SD_ACMD41               (0x40+41)   // SD_SEND_OP_COND (SDC)
  #define SD_CMD1                 (0x40+1)    // SD_SEND_OP_COND (MMC) / activate the card's initialization process
  #define SD_CMD9                 (0x40+9)    // SEND_CSD / asks the selected card to send its card-specific data (CSD)
  #define SD_CMD10                (0x40+10)   // SEND_CID / asks the selected card to send its card identification (CID)
  #define SD_CMD12                (0x40+12)   // STOP_TRANSMISSION / force the card to stop transmission in Mutliple Block Read Operation
  #define SD_ACMD13               (0x40+13)   // SD_STATUS (SDC) / asks the selected card to send its status register
  #define SD_CMD16                (0x40+16)   // SET_BLOCKLEN
  #define SD_CMD17                (0x40+17)   // READ_SINGLE_BLOCK
  #define SD_CMD18                (0x40+18)   // READ_MULTIPLE_BLOCK
  #define SD_CMD23                (0x40+23)   // SET_BLOCK_COUNT (MMC)
  #define SD_ACMD23               (0x40+23)   // SET_WR_BLK_ERASE_COUNT (SDC)
  #define SD_CMD24                (0x40+24)   // WRITE_BLOCK
  #define SD_CMD25                (0x40+25)   // WRITE_MULTIPLE_BLOCK
  #define SD_CMD59                (0x40+59)   // CRC_ON_OFF

  #define SD_ATTEMPTS_R1          0x08
  #define SD_ATTEMPTS_CMD0        0xff
  #define SD_ATTEMPTS_CMD8        0xff
  #define SD_ATTEMPTS_CMD55       0xff

  #define SD_R1_CARD_READY        0x00
  #define SD_R1_IDLE_STATE        0x01
  #define SD_R1_ERASE_RESET       0x02
  #define SD_R1_ILL_COMMAND       0x04
  #define SD_R1_COM_CRC_ERR       0x08
  #define SD_R1_ERASE_SEQ_ERR     0x10
  #define SD_R1_ADDRESS_ERR       0x20
  #define SD_R1_PARAMETER_ERR     0x40

  #define SD_CMD8_VOLT_27_36_V    0x01

  typedef struct {
    uint8_t cmd8_voltage_accept;
  } SD;

  /**
   * @brief   SD Card Init
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SD_Init (void);

  /**
   * @brief   SD Card Power Up Sequence
   *
   * @param   void
   *
   * @return  void
   */
  void SD_PowerUp (void);

  /**
   * @brief   SD Card Power Up Sequence
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SD_IdleState (void);

  /**
   * @brief   SD Send If Condition
   *
   * @param   uint8_t *
   *
   * @return  uint8_t
   */
  uint8_t SD_SendIfCondition (uint8_t *);

  /**
   * @brief   SD Send Application Command
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SD_SendAppCommand (void);

  /**
   * @brief   SD Send Op Condition
   *
   * @param   uint8_t *
   *
   * @return  uint8_t
   */
  uint8_t SD_SendOpCondition (uint8_t *);

  /**
   * @brief   SD Send Application Command
   *
   * @param   uint8_t *
   *
   * @return  uint8_t
   */
  uint8_t SD_ReadOCR (uint8_t * );

  /**
   * @brief   SD Card Response R1
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SD_GetResponseR1 (void);

  /**
   * @brief   SD Card Response R3
   *
   * @param   uint8_t * r
   *
   * @return  void
   */
  uint8_t SD_GetResponseR3 (uint8_t *);

  /**
   * @brief   SD Card Response R7
   *
   * @param   uint8_t *
   *
   * @return  uint8_t
   */
  uint8_t SD_GetResponseR7 (uint8_t *);

  /**
   * @brief   SD Card Send Command
   *
   * @param   uint8_t
   * @param   uint32_t
   * @param   uint8_t
   *
   * @return  void
   */
  void SD_SendCommand (uint8_t, uint32_t, uint8_t);

#endif
