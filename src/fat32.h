/**
 * --------------------------------------------------------------------------------------+
 * @brief       FAT32
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2023 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        01.12.2023
 * @file        fat32.h
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend      avr/io.h, spi.h, sd.h
 * --------------------------------------------------------------------------------------+
 * @interface   SPI 4-wire
 * @pins
 *
 * @sources
 */

#ifndef __FAT32_H__
#define __FAT32_H__

  // RETURN
  // ------------------------------------------------------------------
  #define FAT32_ERROR                   0xff
  #define FAT32_SUCCESS                 0x00

  // Master Boot Record 
  // --------------------------------------------------------------------------------------
  #define MBR_SIGNATURE                 0x55AA

  // Partition Type used in the partition record
  //
  // @src https://d1.amobbs.com/bbs_upload782111/files_2/armok0150242.pdf
  //      https://www.pjrc.com/tech/8051/ide/fat32.html
  //      https://www.richud.com/wiki/FAT32_Filesystem_Slice_Design_Card
  //      https://en.wikipedia.org/wiki/Master_boot_record#PTE
  // --------------------------------------------------------------------------------------
  #define PE_STATUS_ACTIVE_FLAG         0x7F

  #define PE_TYPECODE_UNKNOWN           0x00
  #define PE_TYPECODE_FAT12             0x01
  #define PE_TYPECODE_XENIX             0x02
  #define PE_TYPECODE_DOSFAT16          0x04
  #define PE_TYPECODE_EXTDOS            0x05
  #define PE_TYPECODE_FAT16             0x06
  #define PE_TYPECODE_NTFS              0x07
  #define PE_TYPECODE_FAT32             0x0B
  #define PE_TYPECODE_FAT32LBA          0x0C
  #define PE_TYPECODE_FAT16LBA          0x0E
  #define PE_TYPECODE_EXTDOSLBA         0x0F
  #define PE_TYPECODE_ONTRACK           0x33
  #define PE_TYPECODE_NOVELL            0x40
  #define PE_TYPECODE_PCIX              0x4B
  #define PE_TYPECODE_PHOENIXSAVE       0xA0
  #define PE_TYPECODE_CPM               0xDB
  #define PE_TYPECODE_DBFS              0xE0
  #define PE_TYPECODE_BBT               0xFF

  #define BYTES_PER_SECTOR              0x0200          // 512 Bytes

  // Partition Entry PE
  typedef struct s_PE {
    uint8_t status;                                     // status: 0x80-active, 0x00-inactive
    uint8_t CHSBegin[3];                                // Cylinder/Head/Sector start
    uint8_t typeCode;                                   // partition type: 0x0E-FAT16 LBA, 0x0B-FAT32, 0x0C-FAT32 LBA
    uint8_t CHSEnd[3];                                  // Cylinder/Head/Sector end
    uint8_t LBABegin[4];                                // LBA of first sector
    uint8_t numOfSectors[4];                            // numbers of sectors in partition
  } __attribute__((packed)) s_PE;

  // Master Boot Record MBR
  typedef struct s_MBR {
    uint8_t bootstrap[446];
    s_PE partition1;                                    // 16 bytes Partition Entry
    s_PE partition2;                                    // 16 bytes Partition Entry
    s_PE partition3;                                    // 16 bytes Partition Entry
    s_PE partition4;                                    // 16 bytes Partition Entry
    uint8_t signature[2];                               // signature => must be 0xAA55
  } __attribute__((packed)) s_MBR;

  // Boot Sector (or Volume ID or Block Parameter BIOS)
  typedef struct s_VID {
    uint8_t jumpCode[3];
    uint8_t oemName[8];
    uint8_t bytesPerSector[2];                          // offset 0x0B
    uint8_t sectorsPerCluster;                          // offset 0x0D
    uint8_t reservedSectors[2];                         // offset 0x0E
    uint8_t numOfFat;                                   // offset 0x10
    uint8_t empty[509];
    uint8_t signature[2];                               // offset 0x1FE - signature => must be 0xAA55
  } __attribute__((packed)) s_VID;

  /**
   * @brief   FAT32 Init
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t FAT32_Init (void);

  /**
   * @brief   Read Master Boot Record
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t FAT32_Master_Boot_Record (void);

  /**
   * @brief   Get 4 Bytes Little Endian
   *
   * @param   uint8_t * number
   *
   * @return  uint32_t
   */
  uint8_t FAT32_Get_Uint32_LE (uint8_t *);

#endif
