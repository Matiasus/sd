/**
 * --------------------------------------------------------------------------------------+
 * @brief       FAT32 Interface
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2024 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        16.01.2024
 * @file        fat32.c
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend      sd.h, fat32.h
 * --------------------------------------------------------------------------------------+
 * @interface   SPI
 * @pins        MOSI, MISO, CLK, SS, UCC, USS
 *
 * @sources     http://www.rjhcoding.com/avrc-sd-interface-1.php
 */

// INCLUDE libraries
// ------------------------------------------------------------------
#include "sd.h"
#include "fat32.h"

/**
 * @brief   FAT32 Init
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t FAT32_Init (void)
{
  // init SD structure
  // -------------------------------------------------------------------------------------  
  SD sd = { .voltage = 0, .sdhc = 0, .version = 0 };

  // init SD Card
  // -------------------------------------------------------------------------------------
  if (SD_Init (&sd) == SD_ERROR) {
    return FAT32_ERROR;
  }

  return FAT32_SUCCESS;
}

/**
 * @brief   Read Master Boot Record
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t FAT32_Master_Boot_Record (void)
{
  char str[10];
  uint8_t buffer[512];
  uint16_t signature;
  uint32_t lba_begin;

  // Read MBR
  // ----------------------------------------------------------------
  SD_Read_Block (0, buffer);
  s_MBR * MBR = (s_MBR *) buffer;

  signature = (MBR->signature[0] << 8) | (MBR->signature[1]);                 // signature

  // Check signature 0x55AA
  // ----------------------------------------------------------------
  if (signature != MBR_SIGNATURE) {
    return FAT32_ERROR;
  }
  // Only 0x80 or 0x00 status accepted
  // ----------------------------------------------------------------
  if (MBR->partition1.status & PE_STATUS_ACTIVE_FLAG) {
    return FAT32_ERROR;
  }
  // Only FAT32 or FAT32LBA type code accepted
  // ----------------------------------------------------------------
  if ((MBR->partition1.typeCode != PE_TYPECODE_FAT32) &&
      (MBR->partition1.typeCode != PE_TYPECODE_FAT32LBA)) {
    return FAT32_ERROR;
  }
  // Calculate LBA begin address
  // ----------------------------------------------------------------
  uint32_t lba;
  lba = ((((unsigned long) MBR->partition1.LBABegin[3])<<24) & 0xFF000000) |
        ((((unsigned long) MBR->partition1.LBABegin[2])<<16) & 0x00FF0000) |
        ((((unsigned long) MBR->partition1.LBABegin[1])<< 8) & 0x0000FF00) |
        ((((unsigned long) MBR->partition1.LBABegin[0])<< 0) & 0x000000FF) ;

  SSD1306_SetPosition (2, 2);
  SSD1306_DrawString ("TYPE: 0x", NORMAL);
  sprintf (str, "%02x", MBR->partition1.typeCode);
  SSD1306_DrawString (str, NORMAL);

  SSD1306_SetPosition (2, 3);
  SSD1306_DrawString ("LBA:  0x", NORMAL);
  sprintf (str, "%08x", (unsigned int) lba);
  SSD1306_DrawString (str, NORMAL);

  // Read Volume ID or Boot Sector or Block Parameter Bios
  // ----------------------------------------------------------------
  SD_Read_Block (lba, buffer);
  s_VID * VID = (s_VID *) buffer;

  return FAT32_SUCCESS;
}

/**
 * @brief   Get 4 Bytes Little Endian
 *
 * @param   uint8_t * number
 *
 * @return  uint32_t
 */
uint8_t FAT32_Get_Uint32_LE (uint8_t * n)
{
  uint32_t number = (((unsigned long) n[3] << 24) & 0xFF000000) |
                    (((unsigned long) n[2] << 16) & 0x00FF0000) | 
                    (((unsigned long) n[1] <<  8) & 0x0000FF00) | 
                    (((unsigned long) n[0] <<  0) & 0x000000FF) ;
  return number;
}
