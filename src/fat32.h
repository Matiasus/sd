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
  // @src https://d1.amobbs.com/bbs_upload782111/files_2/armok0150242.pdf
  // --------------------------------------------------------------------------------------
  #define PART_TYPE_UNKNOWN           0x00
  #define PART_TYPE_FAT12             0x01
  #define PART_TYPE_XENIX             0x02
  #define PART_TYPE_DOSFAT16          0x04
  #define PART_TYPE_EXTDOS            0x05
  #define PART_TYPE_FAT16             0x06 
  #define PART_TYPE_NTFS              0x07
  #define PART_TYPE_FAT32             0x0B
  #define PART_TYPE_FAT32LBA          0x0C
  #define PART_TYPE_FAT16LBA          0x0E
  #define PART_TYPE_EXTDOSLBA         0x0F
  #define PART_TYPE_ONTRACK           0x33
  #define PART_TYPE_NOVELL            0x40
  #define PART_TYPE_PCIX              0x4B
  #define PART_TYPE_PHOENIXSAVE       0xA0
  #define PART_TYPE_CPM               0xDB
  #define PART_TYPE_DBFS              0xE0
  #define PART_TYPE_BBT               0xFF

  // MASTER BOOT RECORD
  typedef struct MBR {
    uint8_t bootstrap[446];
    uint8_t partition1[16];
    uint8_t partition2[16];
    uint8_t partition3[16];
    uint8_t partition4[16];
    uint8_t signature[2];
  } __attribute__((packed)) MBR;
  
  // PARTITION ENTRY
  typedef struct PE {
    uint8_t status[1];                                  // status: 0x80-active, 0x00-inactive
    uint8_t chsBegin[3];
    uint8_t typeCode[1];                                // partition type: 0x0E-FAT16 LBA, 0x0C-FAT32 LBA
    uint8_t chsEnd[3];
    uint8_t lbaBegin[4];                                // LBA of first sector
    uint8_t noOfSectors[4];                             // numbers of sectors in partition
  } __attribute__((packed)) PE;

#endif