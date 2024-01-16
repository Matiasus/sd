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
 * @brief   Read Master Boot Record
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t FAT32_Master_Boot_Record (void)
{
  uint8_t buffer[512];
  uint32_t lba_begin;
  uint32_t sectors;

  // Read MBR
  // ----------------------------------------------------------------
  SD_Read_Block (0, buffer);
  s_MBR * MBR = (s_MBR *) buffer;

  // Only 0x80 or 0x00 status accepted
  // ----------------------------------------------------------------
  if (MBR->partition1.status & PE_STATUS_ACTIVE_FLAG) {
    return FAT32_ERROR;
  }
  // Only FAT32 or FAT32LBA type code accepted
  // ----------------------------------------------------------------
  if (MBR->partition1->typeCode != PE_TYPECODE_FAT32 ||
      MBR->partition1->typeCode != PE_TYPECODE_FAT32LBA) {
    return FAT32_ERROR;
  }
  // Calculate LBA begin address
  // ----------------------------------------------------------------
  lba_begin = FAT32_Get_Uint32_LE (MBR->partition1.LBABegin) * BYTES_PER_SECTOR;

  // Read Volume ID or Boot Sector or Block Parameter Bios
  // ----------------------------------------------------------------
  SD_Read_Block (lba_begin, buffer);
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
  uint32_t number = n[3] << 24 & 0xFF000000 | n[2] << 16 & 0x00FF0000 | n[1] << 8 & 0x0000FF00 | n[0] << 0 & 0x000000FF;
  return number;
}
