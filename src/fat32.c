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
  uint8_t buffer[512];
  uint32_t lba_begin;

  // Read MBR / Master Boot Record
  // ----------------------------------------------------------------
  SD_Read_Block (0, buffer);
  s_MBR * MBR = (s_MBR *) buffer;
  
  // Checking
  // ----------------------------------------------------------------
  if ((FAT32_Get_2Bytes_LE (MBR->Signature) != FAT32_SIGNATURE)) {            // check signature 0xAA55
    return FAT32_ERROR;
  }
  if (MBR->partition1.Status & PE_STATUS_ACTIVE_FLAG) {                       // only 0x80 or 0x00 status accepted
    return FAT32_ERROR;
  }
  if ((MBR->partition1.TypeCode != PE_TYPECODE_FAT32) &&                      // only FAT32 or FAT32LBA type code accepted
      (MBR->partition1.TypeCode != PE_TYPECODE_FAT32LBA)) {
    return FAT32_ERROR;
  }

  // LBA Begin Address
  // ----------------------------------------------------------------
  lba_begin = FAT32_Get_4Bytes_LE (MBR->partition1.LBA_Begin);                // LBA begin address

  return lba_begin;
}

/**
 * @brief   Read Master Boot Record
 *
 * @param   uint32_t
 *
 * @return  uint32_t
 */
uint32_t FAT32_Boot_Sector (uint32_t lba_begin)
{
  uint16_t reserved_sectors;
  uint32_t fat_begin;
  uint32_t data_begin;
  uint32_t root_dir_clus_no;
  
  // Read Boot Sector with BIOS Parameter Block Parameter Bios
  // ----------------------------------------------------------------
  SD_Read_Block (lba_begin * BYTES_PER_SECTOR, buffer);
  s_BS * BS = (s_BS *) buffer;

  // Checking
  // ----------------------------------------------------------------
  if ((FAT32_Get_2Bytes_LE (BS->Signature) != FAT32_SIGNATURE)) {             // check signature 0xAA55
    return FAT32_ERROR;
  }
  if ((FAT32_Get_2Bytes_LE (BS->BytesPerSector) != BYTES_PER_SECTOR)) {       // only 512 bytes per sector accepted
    return FAT32_ERROR;
  }
  if (BS->NumberOfFATs != FAT32_NUM_OF_FATS) {                                // only 2 FAT tables accepted
    return FAT32_ERROR;
  }

  // Calculation
  // ----------------------------------------------------------------
  reserved_sectors = FAT32_Get_2Bytes_LE (VID->ReservedSectors);
  sector_per_fats = FAT32_Get_4Bytes_LE (BS->BigSectorsPerFAT);
  root_dir_clus_no = FAT32_Get_4Bytes_LE (BS->RootDirClusNo);
  
  fat_begin = lba_begin + reserved_sectors;
  data_begin = fat_begin + (BS->NumberOfFATs * sector_per_fats);
  root_dir_starts = data_begin + ((root_dir_clus_no - 2) * BS->SectorsPerCluster);
  
  return root_dir_starts;
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
