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
  #include <stdio.h>
  #include <avr/io.h>
  #include <util/delay.h>
  #include "../spi/spi.h"
  #include "../lcd/ssd1306.h"

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

  #define SD_DDR_CS               DDRB
  #define SD_PORT_CS              PORTB
  #define SD_CS                   1

  // SD CARD COMMAND TABLE
  // ------------------------------------------------------------------
  #define SD_CMD0                 (0x40+0)    // GO_IDLE_STATE / Reset the SD Memory Card
  #define SD_CMD0_ARG             0x00000000
  #define SD_CMD0_CRC             0x95

  #define SD_CMD8                 (0x40+8)    // SEND_IF_COND / voltage information and asks the accessed card whether card can operate in supplied voltage range
  #define SD_CMD8_ARG             0x000001AA  // check pattern AA / Phzsical Layer Spec Version 3.01 page 62
  #define SD_CMD8_CRC             0x87
 
  #define SD_CMD55                (0x40+55)   // APP_CMD
  #define SD_CMD55_ARG            0x00000000  // 
  #define SD_CMD55_CRC            0x00        // CRC

  #define SD_ACMD41               (0x40+41)   // SD_SEND_OP_COND (SDC) / Activates the card’s initialization process
  #define SD_ACMD41_ARG           0x40000000  // 0x40000000 -> HCS=1 SDHC or SDXC Supported, 0x00000000 -> HCS=0 SDSC Only Host
  #define SD_ACMD41_CRC           0x00        // CRC

  #define SD_CMD58                (0x40+58)   // READ_OCR
  #define SD_CMD58_ARG            0x00000000  //
  #define SD_CMD58_CRC            0x00        // CRC

  #define SD_CMD1                 (0x40+1)    // SD_SEND_OP_COND (MMC) / Activate the card's initialization process
  #define SD_CMD1_ARG             0x00000000  //
  #define SD_CMD1_CRC             0x00        // CRC

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

  #define SD_R1                   1
  #define SD_R3                   5
  #define SD_R7                   5

  #define SD_ATTEMPTS_R1          0x08
  #define SD_ATTEMPTS_CMD0        0xff
  #define SD_ATTEMPTS_CMD1        0xff
  #define SD_ATTEMPTS_CMD8        0xff
  #define SD_ATTEMPTS_CMD55       0xff
  #define SD_ATTEMPTS_CMD17       1563

  #define SD_R1_CARD_READY        0x00
  #define SD_R1_IDLE_STATE        0x01
  #define SD_R1_ERASE_RESET       0x02
  #define SD_R1_ILL_COMMAND       0x04
  #define SD_R1_COM_CRC_ERR       0x08
  #define SD_R1_ERASE_SEQ_ERR     0x10
  #define SD_R1_ADDRESS_ERR       0x20
  #define SD_R1_PARAMETER_ERR     0x40

  #define SD_CMD8_VOLT_27_36_V    0x01
  #define SD_CMD58_READY          0x80
  #define SD_CMD58_CCS            0x40

  #define SD_SDHC_BLOCKLEN        512
  
  typedef struct SD {
    uint8_t voltage;                          // 0 - rejected, 1 - accepted / CMD8
    uint8_t sdhc;                             // 0 - unknown, 1 - SDSC, 2 - SDHC or SDXC / CMD58
    uint8_t version;                          // 0 - unknown, 1 - SD Ver.2+ (Block Address), 2 - SD Ver.2+, 3 - SD Ver.1, 4 - MMC Ver.3
  } SD;

  /**
   * @brief   SD Card Init
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SD_Init (SD *);

  /**
   * @brief   SD Card Read Data
   *
   * @param   uint32_t
   * @param   uint8_t *
   *
   * @return  uint8_t
   */
  uint8_t SD_Read_Block (uint32_t, uint8_t *);

  /**
   * @brief   SD Card Power Up Sequence
   *
   * @param   void
   *
   * @return  void
   */
  void SD_Power_Up (void);

  /**
   * @brief   SD Send SD Command
   *
   * @param   uint8_t command 
   * @param   uint32_t argument
   * @param   uint8_t crc
   * @param   uint8_t * response
   * @param   uint8_t n-times
   *
   * @return  uint8_t
   */
  uint8_t SD_Send_CMDx (uint8_t, uint32_t, uint8_t, uint8_t *, uint8_t);

  /**
   * @brief   SD Card Response R1
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SD_Get_Response_R1 (void);

  /**
   * @brief   SD Card Response R1, R3, R7
   *
   * @param   uint8_t * response
   * @param   uint8_t n-times
   *
   * @return  uint8_t
   */
  uint8_t SD_Get_Response_Rn (uint8_t *, uint8_t);

  /**
   * @brief   SD Card Send Command
   *
   * @param   uint8_t
   * @param   uint32_t
   * @param   uint8_t
   *
   * @return  void
   */
  void SD_Send_Command (uint8_t, uint32_t, uint8_t);

#endif
