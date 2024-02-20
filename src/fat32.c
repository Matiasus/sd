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
 * @interface   SPI554          0
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
 * @param   FAT32_t * FAT32 structure
 *
 * @return  uint8_t
 */
uint8_t FAT32_Init (FAT32_t * FAT32)
{
  // SD Card Init
  // -------------------------------------------------------------------------------------
  SD sd = { .voltage = 0, .sdhc = 0, .version = 0 };
  if (SD_Init (&sd) == SD_ERROR) {
    return FAT32_ERROR;
  }

  // MBR - Read Master Boot Record
  // ----------------------------------------------------------------
  if (FAT32_ERROR == FAT32_Read_Master_Boot_Record (FAT32)) {
    return FAT32_ERROR;
  }
  // BS - Read Boot Sector
  // ----------------------------------------------------------------
  if (FAT32_ERROR == FAT32_Read_Boot_Sector (FAT32)) {
    return FAT32_ERROR;
  }

  return FAT32_SUCCESS;
}

/**
 * @brief   Read Master Boot Record
 *
 * @param   FAT32_t * FAT32
 *
 * @return  uint8_t
 */
uint8_t FAT32_Read_Master_Boot_Record (FAT32_t * FAT32)
{
  uint8_t buffer[512];

  // Read MBR / Master Boot Record
  // ----------------------------------------------------------------
  SD_Read_Block (0, buffer);
  MBR_t * MBR = (MBR_t *) buffer;
  
  // Checking
  // ----------------------------------------------------------------
  if ((FAT32_Get_2Bytes_LE (MBR->Signature) != FAT32_SIGNATURE)) {            // check signature 0xAA55
    return FAT32_ERROR;
  }
  if (MBR->Partition1.Status & PE_STATUS_ACTIVE_FLAG) {                       // only 0x80 or 0x00 status accepted
    return FAT32_ERROR;
  }
  if ((MBR->Partition1.TypeCode != PE_TYPECODE_FAT32) &&                      // only FAT32 or FAT32LBA type code accepted
      (MBR->Partition1.TypeCode != PE_TYPECODE_FAT32LBA)) {
    return FAT32_ERROR;
  }

  // LBA Begin Address
  // ----------------------------------------------------------------
  FAT32->lba_begin = FAT32_Get_4Bytes_LE (MBR->Partition1.LBA_Begin);         // LBA begin address

  return FAT32_SUCCESS;
}

/**
 * @brief   Read Master Boot Record
 *
 * @param   FAT32_t * FAT32
 *
 * @return  uint8_t
 */
uint8_t FAT32_Read_Boot_Sector (FAT32_t * FAT32)
{
  uint8_t buffer[512];
  uint16_t reserved_sectors;
  uint32_t sector_per_fats;
  uint32_t root_dir_clus;

  // Read Boot Sector with BIOS Parameter Block
  // ----------------------------------------------------------------
  SD_Read_Block (FAT32->lba_begin, buffer);                                   // 2048 = 0x00000800
  BS_t * BS = (BS_t *) buffer;

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

  // Calculations
  // ----------------------------------------------------------------
  reserved_sectors = FAT32_Get_2Bytes_LE (BS->ReservedSectors);
  sector_per_fats = FAT32_Get_4Bytes_LE (BS->BigSectorsPerFAT);
  root_dir_clus = FAT32_Get_4Bytes_LE (BS->RootDirClusNo);

  FAT32->root_dir_clus_num = root_dir_clus;
  FAT32->sectors_per_cluster = BS->SectorsPerCluster;
  FAT32->fat_area_begin = FAT32->lba_begin + reserved_sectors;
  FAT32->data_area_begin = FAT32->fat_area_begin + (BS->NumberOfFATs * sector_per_fats);

  return FAT32_SUCCESS;
}

/**
 * @brief   Read Root Directory
 *
 * @param   FAT32_t * FAT32
 *
 * @return  uint32_t
 *  */
uint32_t FAT32_Root_Dir_Files (FAT32_t * FAT32)
{
  DE_t * DE;
  uint8_t sectors = FAT32->sectors_per_cluster;                               // number of sectors in cluster;
  uint8_t buffer[BYTES_PER_SECTOR];

  uint32_t sector;
  uint32_t files = 0;
  uint32_t cluster = FAT32->root_dir_clus_num;                                // next cluster of root directory

  do {

    sector = FAT32_Get_1st_Sector_Of_Clus (FAT32, cluster);                   // 1st sector of cluster

    // Read Cluster
    // ----------------------------------------------------------------    
    while (sectors--) {
      // Read Sector
      // --------------------------------------------------------------
      SD_Read_Block (sector++, buffer);
      // Read Root Directory Entries
      // --------------------------------------------------------------
      for (uint16_t i=0; i<BYTES_PER_SECTOR; i=i+32) {
        DE = (DE_t *) &buffer[i];
        if (DE->Name[0] == FAT32_DE_END) {      
          return files;                                                       // end of files
        } else if (DE->Name[0] != FAT32_DE_UNUSED) {                          // deleted files
          if (((DE->Name[0] >> 4) != 0x04) &&                                 // end of long file name
              (DE->Name[0] > 0x20)) {                                         // !!! except 0x05
            files++;                                                          // count files
          }
        }
      }
    }

    cluster = FAT32_FAT_Next_Cluster (FAT32, cluster);                        // get next cluster
    cluster &= 0x0FFFFFFF;                                                    // mask first nibble

  } while (cluster < 0x0FFFFFF8);                                             // 0x?ffffff8 - 0x?fffffff = Last cluster in file (EOC)

  return files;
}

/**
 * @brief   Read Next Cluster From FAT
 *
 * @param   FAT32_t * FAT32
 * @param   uint32_t cluster position in FAT
 *
 * @return  uint32_t
 */
uint32_t FAT32_FAT_Next_Cluster (FAT32_t * FAT32, uint32_t cluster_pos_in_FAT)
{
  uint8_t buffer[512];

  uint32_t next_cluster;
  uint32_t packet = cluster_pos_in_FAT << 2;                                  // sequel * 4
  uint32_t sector = FAT32->fat_area_begin + packet / BYTES_PER_SECTOR;        // fats_begin + next block for SD read
  uint32_t offset = packet - (sector * BYTES_PER_SECTOR);                     // packet % 512

  // Read Directory Entries
  // ----------------------------------------------------------------
  SD_Read_Block (sector, buffer);
  next_cluster = FAT32_Get_4Bytes_LE (&buffer[offset]);

  return next_cluster;
}

/**
 * @brief   Get Address (Offset) Of 1st Sector Of Cluster Number
 *
 * @param   FAT32_t * FAT32
 * @param   uint32_t cluster number
 *
 * @return  uint32_t
 *  */
uint32_t FAT32_Get_1st_Sector_Of_Clus (FAT32_t * FAT32, uint32_t cluster)
{
  return (FAT32->data_area_begin + ((cluster - FAT32->root_dir_clus_num) * FAT32->sectors_per_cluster));
}

/**
 * @brief   Get File Info from Root Directory
 *
 * @param   FAT32_t * FAT32
 * @param   uint32_t file number
 *
 * @return  DE_t * => directory entry
 *  */
DE_t * FAT32_Get_File_Info (FAT32_t * FAT32, uint32_t filenum)
{
  DE_t * DE;
  uint8_t sectors = FAT32->sectors_per_cluster;                               // number of sectors in cluster
  uint8_t buffer[BYTES_PER_SECTOR];

  uint32_t sector;
  uint32_t files = 0;
  uint32_t cluster = FAT32->root_dir_clus_num;                                // next cluster of root directory

  do {

    sector = FAT32_Get_1st_Sector_Of_Clus (FAT32, cluster);                   // 1st sector of cluster

    // Read Cluster
    // ----------------------------------------------------------------    
    while (sectors--) {
      // Read Sector
      // --------------------------------------------------------------
      SD_Read_Block (sector++, buffer);
      // Read Root Directory Entries
      // --------------------------------------------------------------
      for (uint16_t i=0; i<BYTES_PER_SECTOR; i=i+32) {
        DE = (DE_t *) &buffer[i];
        if (DE->Name[0] == FAT32_DE_END) {      
          return DE;                                                          // end of files
        } else if (DE->Name[0] != FAT32_DE_UNUSED) {                          // deleted files
          if (((DE->Name[0] >> 4) != 0x04) &&                                 // end of long file name
              (DE->Name[0] > 0x20)) {                                         // !!! except 0x05
            if (++files == filenum) {
              return DE;
            }
          }
        }
      }
    }

    cluster = FAT32_FAT_Next_Cluster (FAT32, cluster);                        // get next cluster
    cluster &= 0x0FFFFFFF;                                                    // mask first nibble

  } while (cluster < 0x0FFFFFF8);                                             // 0x?ffffff8 - 0x?fffffff = Last cluster in file (EOC)

  return FAT32_SUCCESS;
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
