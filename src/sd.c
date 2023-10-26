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
uint8_t SD_Init (SD * sd)
{
  uint8_t r[5];
  uint8_t attempt;

  // SPI Init (cs, settings)
  // ----------------------------------------------------------------
  SPI_Init (SPI_SS, SPI_MASTER | SPI_MODE_0 | SPI_MSB_FIRST | SPI_FOSC_DIV_128);

  // Power Up 
  // ----------------------------------------------------------------
  SD_Power_Up ();
 
  // Idle State - CMD0 
  // ----------------------------------------------------------------
  attempt = 0;
  while (SD_Send_CMDx (SD_CMD0, SD_CMD0_ARG, SD_CMD0_CRC, r, SD_R1) != SD_R1_IDLE_STATE) {
    if (++attempt > SD_ATTEMPTS_CMD0) {
      return SD_ERROR;
    }
  }
  // Send If Condition - CMD8
  // ----------------------------------------------------------------
  SD_Send_CMDx (SD_CMD8, SD_CMD8_ARG, SD_CMD8_CRC, r, SD_R7);
  if ((r[4] != (uint8_t) SD_CMD8_ARG) ||                // check sent pattern
      (r[3] != SD_CMD8_VOLT_27_36_V)) {                 // check voltage
    return SD_ERROR;
  } 
  else if (r[0] == SD_R1_IDLE_STATE) {                  // check idle state    
    sd->voltage_accept = 1;                             // accepted voltage range 2,7-3,6V
    // Send If Comdition - ACMD41 for SD Ver.2+
    // --------------------------------------------------------------
    attempt = 0;
    do {
      SD_Send_CMDx (SD_CMD55, SD_CMD55_ARG, SD_CMD55_CRC, r, SD_R1);
      if ((r[0] == SD_R1_CARD_READY) || 
          (r[0] == SD_R1_IDLE_STATE)) {
        SD_Send_CMDx (SD_ACMD41, SD_ACMD41_ARG, SD_ACMD41_CRC, r, SD_R1);
      }
      if (++attempt > SD_ATTEMPTS_CMD55) {
        return SD_ERROR;
      }
      _delay_ms (1);
    } while (r[0] != SD_R1_CARD_READY);
    // Read OCR - CMD58
    // --------------------------------------------------------------
    SD_Send_CMDx (SD_CMD58, SD_CMD58_ARG, SD_CMD58_CRC, r, SD_R3);
    if ((r[0] != SD_R1_CARD_READY)) {                   // card ready
      return SD_ERROR;
    }
    if (r[1] & SD_CMD58_READY) {
      sd->sdhc = 2;                                     // SDHC or SDXC
      if (r[1] & SD_CMD58_CCS) {
        sd->version = 1;                                // SD Ver.2+ Block Address
      } else {
        sd->version = 2;                                // SD Ver.2+ Byte Address
      }
    }
  } else {
    // Send If Comdition - ACMD41 for SD Ver.1
    // --------------------------------------------------------------
    attempt = 0;
    do {
      SD_Send_CMDx (SD_CMD55, SD_CMD55_ARG, SD_CMD55_CRC, r, SD_R1);
      if ((r[0] == SD_R1_CARD_READY) || 
          (r[0] == SD_R1_IDLE_STATE)) {
        SD_Send_CMDx (SD_ACMD41, 0x00000000, SD_ACMD41_CRC, r, SD_R1);
      }
      if (++attempt > SD_ATTEMPTS_CMD55) {
        break;
      }
      _delay_ms (1);
    } while (r[0] != SD_R1_CARD_READY);

    if (r[0] == SD_R1_CARD_READY) {
      sd->version = 3;                                  // SD Ver.1 Byte Address
    } else {
      while (SD_Send_CMDx (SD_CMD1, SD_CMD1_ARG, SD_CMD0_CRC, r, SD_R1) != SD_R1_CARD_READY) {
        if (++attempt > SD_ATTEMPTS_CMD1) {
          return SD_ERROR;
        }
      }
      sd->version = 4;                                  // MMC Ver.3 Byte Address      
    }
  }

  sd->success = 1;                                      // SD card initialized
  
  return SD_SUCCESS;
}


/**
 * @brief   SD Card Power Up Sequence
 *
 * @param   void
 *
 * @return  void
 */
void SD_Power_Up (void)
{
  // Power Up Time Delay
  // ----------------------------------------------------------------
  _delay_ms(250);                                       // power up time

  // Supply Ram Up Sequence
  // ----------------------------------------------------------------
  CS_DISABLE();                                         // hold CS high
  _delay_ms(1);                                         // supply ramp up time

  for (uint8_t i=0; i<10; i++) {                        // supply ramp up cycles
    SPI_Transfer (0xff);                                // min 74 cycles
  }
  // Deselect Card
  // accor. http://www.rjhcoding.com/avrc-sd-interface-1.php
  // ----------------------------------------------------------------
  CS_DISABLE();
  SPI_Transfer (0xff);
}

/**
 * @brief   SD Send SD Command
 *
 * @param   uint8_t
 * @param   uint32_t
 * @param   uint8_t
 * @param   uint8_t *
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SD_Send_CMDx (uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t * r, uint8_t n)
{
  SPI_Transfer (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_Transfer (0xff);

  // === R1 response ===
  // ----------------------------------------------------------------
  SD_Send_Command (cmd, arg, crc);
  uint8_t response = SD_Get_Response_Rn (r, n);

  SPI_Transfer (0xff);
  CS_DISABLE ();                                        // CS high
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
uint8_t SD_Get_Response_R1 (void)
{
  uint8_t i = 0;
  uint8_t response = 0xff;

  while ((response = SPI_Transfer (0xff)) == 0xff) {
    if (++i > SD_ATTEMPTS_R1) {
      break;
    }
  }

  return response;
}

/**
 * @brief   SD Card Response R1, R3, R7
 *
 * @param   uint8_t * response
 * @param   uint8_t n-times
 *
 * @return  uint8_t
 */
uint8_t SD_Get_Response_Rn (uint8_t * r, uint8_t n)
{
  for (uint8_t i=0; i<n; i++) {
    r[i] = SD_Get_Response_R1 ();
  }

  return r[0];
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
void SD_Send_Command (uint8_t cmd, uint32_t arg, uint8_t crc)
{
  SPI_Transfer (0x40 | cmd);                           // send command
  SPI_Transfer ((uint8_t) (arg >> 24));                // send arguments
  SPI_Transfer ((uint8_t) (arg >> 16));                //
  SPI_Transfer ((uint8_t) (arg >>  8));                //
  SPI_Transfer ((uint8_t) (arg >>  0));                //
  SPI_Transfer (crc | 0x01);                           // cyclic redundancy check
}
