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
 * @return  uint32_t
 */
uint32_t FAT32_Master_Boot_Record (void)
{
  char str[10];
  uint8_t buffer[512];
  uint32_t lba_begin;
  uint32_t volume_id;

  // Read MBR / Master Boot Record
  // ----------------------------------------------------------------
  SD_Read_Block (0, buffer);
  s_MBR * MBR = (s_MBR *) buffer;

  lba_begin = FAT32_Get_4Bytes_LE (MBR->partition1.LBABegin);                 // LBA begin address
  volume_id = lba_begin * BYTES_PER_SECTOR;                                   // volume id start address
  
  // Checking
  // ----------------------------------------------------------------
  if ((FAT32_Get_2Bytes_LE (MBR->signature) != FAT32_SIGNATURE) ||            // check FAT32 signature 0x55AA
      (MBR->partition1.status & PE_STATUS_ACTIVE_FLAG)          ||            // only 0x80 or 0x00 status accepted
      ((MBR->partition1.typeCode != PE_TYPECODE_FAT32) &&                     // only FAT32 or FAT32LBA type code accepted
       (MBR->partition1.typeCode != PE_TYPECODE_FAT32LBA))) {                 // 
    return FAT32_ERROR;
  }
  
  // Print
  // ----------------------------------------------------------------
  SSD1306_SetPosition (2, 2);
  SSD1306_DrawString ("TYPE: 0x", NORMAL);
  sprintf (str, "%02x", MBR->partition1.typeCode);
  SSD1306_DrawString (str, NORMAL);

  SSD1306_SetPosition (2, 3);
  SSD1306_DrawString ("LBA:  0x", NORMAL);
  sprintf (str, "%08x", (unsigned int) lba);
  SSD1306_DrawString (str, NORMAL);

  return volume_id;
}

/**
 * @brief   Read Master Boot Record
 *
 * @param   uint32_t
 *
 * @return  uint8_t
 */
uint8_t FAT32_Volume_ID (uint32_t lba_begin)
{
  // Read BPB / Volume ID or Boot Sector or Block Parameter Bios
  // ----------------------------------------------------------------
  SD_Read_Block (lba_begin * , buffer);
  s_VID * VID = (s_VID * BYTES_PER_SECTOR) buffer;

  // Checking
  // ----------------------------------------------------------------
  if ((FAT32_Get_2Bytes_LE (VID->signature) != FAT32_SIGNATURE) ||            // check signature 0x55AA
      (FAT32_Get_Uint16_LE (VID->bytesPerSector) != BYTES_PER_SECTOR) ||      // only 512 Bytes per sector accepted
      (VID->numOfFat != FAT32_NUM_OF_FATS)) {                                 // only 2 FAT tables accepted
    return FAT32_ERROR;
  }
  
  uint8_t sectors_per_cluster = VID->sectorsPerCluster;
  uint16_t reserved_sector = FAT32_Get_Uint16_LE (VID->reservedSectors);
  uint32_t fat_begin = (lba_begin + reserved_sector) * BYTES_PER_SECTOR;
}
/**
 * --------------------------------------------------------------------------------------------+
 * PRIMITIVE / PRIVATE FUNCTIONS
 * --------------------------------------------------------------------------------------------+
 */

/**
 * @brief   Get 2 Bytes Little Endian
 *
 * @param   uint8_t * number
 *
 * @return  uint16_t
 */
uint16_t FAT32_Get_2Bytes_LE (uint8_t * n)
{
  uint32_t number = (((uint16_t) n[1] << 8) & 0xFF00) | 
                    (((uint16_t) n[0] << 0) & 0x00FF) ;
  return number;
}

/**
 * @brief   Get 4 Bytes Little Endian
 *
 * @param   uint8_t * number
 *
 * @return  uint32_t
 */
uint32_t FAT32_Get_4Bytes_LE (uint8_t * n)
{
  uint32_t number = (((uint32_t) n[3] << 24) & 0xFF000000) |
                    (((uint32_t) n[2] << 16) & 0x00FF0000) | 
                    (((uint32_t) n[1] <<  8) & 0x0000FF00) | 
                    (((uint32_t) n[0] <<  0) & 0x000000FF) ;
  return number;
}
