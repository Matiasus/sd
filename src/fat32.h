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

  // Partition Type used in the partition record
  //
  // @src https://d1.amobbs.com/bbs_upload782111/files_2/armok0150242.pdf
  //      https://www.pjrc.com/tech/8051/ide/fat32.html
  //      https://www.richud.com/wiki/FAT32_Filesystem_Slice_Design_Card
  //      https://en.wikipedia.org/wiki/Master_boot_record#PTE
  // --------------------------------------------------------------------------------------
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

  // MASTER BOOT RECORD
  typedef struct MBR {
    uint8_t bootstrap[446];
    PE partition1;                                      // 16 bytes Partition Entry
    PE partition2;                                      // 16 bytes Partition Entry
    PE partition3;                                      // 16 bytes Partition Entry
    PE partition4;                                      // 16 bytes Partition Entry
    uint8_t signature[2];                               // signature => must be 0xAA55
  } __attribute__((packed)) MBR;
  
  // PARTITION ENTRY
  typedef struct PE {
    uint8_t status[1];                                  // status: 0x80-active, 0x00-inactive
    uint8_t CHSBegin[3];                                // Cylinder/Head/Sector start
    uint8_t typeCode[1];                                // partition type: 0x0E-FAT16 LBA, 0x0B-FAT32, 0x0C-FAT32 LBA
    uint8_t CHSEnd[3];                                  // Cylinder/Head/Sector end
    uint8_t LBABegin[4];                                // LBA of first sector
    uint8_t noOfSectors[4];                             // numbers of sectors in partition
  } __attribute__((packed)) PE;

  // BOOT SECTOR
  typedef struct BS {
    uint8_t jumpCode[3];
    uint8_t oemName[8];
    uint8_t bytesPerSector[2];
    uint8_t sectorsPerCluster[1];
    uint8_t reservedSectors[2];
    uint8_t numOfFat[1];
    uint8_t empty[509];
    uint8_t signature[2];                               // signature => must be 0xAA55
  } __attribute__((packed)) BS;

#endif
