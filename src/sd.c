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
  uint8_t r[5];
  uint8_t attempt;

  SD sd_init = { .cmd8_voltage_accept = 0, .cmd58_sdhc = 0 };

  // SPI Init (cs, settings)
  // ----------------------------------------------------------------  
  SPI_Init (SPI_SS, SPI_MASTER | SPI_MODE_0 | SPI_MSB_FIRST | SPI_FOSC_DIV_128);

  // Power Up 
  // ----------------------------------------------------------------
  SD_PowerUp ();

  // Idle State - CMD0 
  // ----------------------------------------------------------------
  attempt = 0;
  while ((r[0] = SD_IdleState ()) != SD_R1_IDLE_STATE) {
    if (++attempt > SD_ATTEMPTS_CMD0) {
      return SD_ERROR;
    }
  }
  // Send If Condition - CMD8
  // ----------------------------------------------------------------
  SD_SendIfCondition (r);
  if (r[0] != SD_R1_IDLE_STATE ||                       // check idle state
      r[4] != (uint8_t) SD_CMD8_ARG) {                  // check sent pattern
    return SD_ERROR;
  }
  if (r[3] == SD_CMD8_VOLT_27_36_V) {
    sd.cmd8_voltage_accept = 1;                         // accepted voltage range 2,7-3,6V
  }
  // ACMD41
  // ----------------------------------------------------------------
  attempt = 0;
  do {
    r[0] = SD_SendAppCommand ();
    if ((r[0] != SD_R1_CARD_READY) || 
        (r[0] != SD_R1_IDLE_STATE)) {
      SD_SendOpCondition (r);
    }
    if (++attempt > SD_ATTEMPTS_CMD55) {
      return SD_ERROR;
    }
    _delay_ms (1);
  } while (r[0] != SD_R1_CARD_READY);

  // Read OCR - CMD58
  // ----------------------------------------------------------------
  SD_ReadOCR (r);
  if ((r[0] != SD_R1_CARD_READY)) {                  // card ready
    return SD_ERROR;
  }
  if (r[1] == SD_CMD58_BUSY &&
      r[1] == SD_CMD58_CCS) {
    sd.cmd58_sdhc = 1;
  }

  return SD_SUCCESS;
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
  // Power Up Time Delay
  // ----------------------------------------------------------------
  //_delay_ms(250);                                       // power up time

  // Supply Ram Up Sequence
  // ----------------------------------------------------------------
  CS_DISABLE();                                         // hold CS high
  _delay_ms(1);                                         // supply ramp up time

  for (uint8_t i=0; i<10; i++) {                        // supply ramp up cycles
    SPI_Transfer (0xff);                               // min 74 cycles
  }
  // Deselect Card
  // accor. http://www.rjhcoding.com/avrc-sd-interface-1.php
  // ----------------------------------------------------------------
  CS_DISABLE();
  SPI_Transfer (0xff);
}

/**
 * @brief   SD Idle State
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SD_IdleState (void)
{
  SPI_Transfer (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_Transfer (0xff);

  SD_SendCommand (SD_CMD0, SD_CMD0_ARG, SD_CMD0_CRC);   // Send CMD0
  uint8_t response = SD_GetResponseR1 ();               // get response R1

  SPI_Transfer (0xff);
  CS_DISABLE ();                                        // CS High
  SPI_Transfer (0xff);

  return response;
}

/**
 * @brief   SD Send If Condition
 *
 * @param   uint8_t * r
 *
 * @return  void
 */
void SD_SendIfCondition (uint8_t * r)
{
  SPI_Transfer (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_Transfer (0xff);

  SD_SendCommand (SD_CMD8, SD_CMD8_ARG, SD_CMD8_CRC);   // Send CMD8
  r = SD_GetResponseR7 ();                              // get response R7

  SPI_Transfer (0xff);
  CS_DISABLE ();                                        // CS High
  SPI_Transfer (0xff);
}

/**
 * @brief   SD Send Application Command
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SD_SendAppCommand (void)
{
  SPI_Transfer (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_Transfer (0xff);

  SD_SendCommand (SD_CMD55, 0x000000, 0x00);            // Send CMD55
  uint8_t response = SD_GetResponseR1 ();               // get response R1

  SPI_Transfer (0xff);
  CS_DISABLE ();                                        // CS High
  SPI_Transfer (0xff);

  return response;
}

/**
 * @brief   SD Send Op Condition
 *
 * @param   uint8_t *
 *
 * @return  uint8_t
 */
uint8_t SD_SendOpCondition (uint8_t * r)
{
  SPI_Transfer (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_Transfer (0xff);

  // Argument of ACMD41 bit 23-0 means "inquiry CMD41", does not start
  // init process but is used for getting OCR
  // cmd = 0x400000 -> HCS=1 SDHC or SDXC Supported
  // cmd = 0x000000 -> HCS=0 SDSC Only Host
  // ----------------------------------------------------------------
  SD_SendCommand (SD_ACMD41, 0x400000, 0x00);           // Send ACMD41
  SD_GetResponseR3 (r);                                 // get response R3

  SPI_Transfer (0xff);
  CS_DISABLE ();                                        // CS High
  SPI_Transfer (0xff);
}

/**
 * @brief   SD Send Application Command
 *
 * @param   uint8_t *
 *
 * @return  uint8_t
 */
uint8_t SD_ReadOCR (uint8_t * r)
{
  SPI_Transfer (0xff);
  CS_ENABLE ();                                         // CS low
  SPI_Transfer (0xff);

  SD_SendCommand (SD_CMD58, 0x000000, 0x00);            // Send CMD55
  SD_GetResponseR3 (r);                                 // get response R3

  SPI_Transfer (0xff);
  CS_DISABLE ();                                        // CS High
  SPI_Transfer (0xff);
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

  while ((response = SPI_Transfer (0xff)) == 0xff) {
    if (++i > SD_ATTEMPTS_R1) {
      break;
    }
  }

  return response;
}

/**
 * @brief   SD Card Response R3
 *
 * @param   uint8_t * r
 *
 * @return  void
 */
uint8_t SD_GetResponseR3 (uint8_t * r)
{
  SD_GetResponseR7 (r);
}

/**
 * @brief   SD Card Response R7
 *
 * @param   uint8_t * r
 *
 * @return  void
 */
uint8_t SD_GetResponseR7 (uint8_t * r)
{
  uint8_t i = 0;
  uint8_t response = 0xff;

  // get R1
  // ----------------------------------------------------------------
  if ((r[0] = SD_GetResponseR1 ()) == SD_R1_IDLE_STATE)
    return SD_ERROR;
  }

  // Get rest of 4 byte answer
  // ----------------------------------------------------------------
  for (i=1; i<5; i++) {
    r[i] = SPI_Transfer (0xff);
  }

  return SD_SUCCESS;
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
  SPI_Transfer (cmd);                                  // send command
  SPI_Transfer ((uint8_t) (arg >> 24));                // send arguments
  SPI_Transfer ((uint8_t) (arg >> 16));                //
  SPI_Transfer ((uint8_t) (arg >>  8));                //
  SPI_Transfer ((uint8_t) (arg >>  0));                //
  SPI_Transfer (crc | 0x01);                           // cyclic redundancy check
}
