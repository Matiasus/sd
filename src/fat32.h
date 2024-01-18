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
  // --------------------------------------------------------------------------------------
  #define FAT32_ERROR                   0xff
  #define FAT32_SUCCESS                 0x00

  // Master Boot Record 
  // --------------------------------------------------------------------------------------
  #define FAT32_SIGNATURE               0x55AA
  #define FAT32_NUM_OF_FATS             2

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
    uint8_t Status;                                     // Boot indicator bit flag: 0 = no, 0x80 = bootable (or "active")
    uint8_t CHS_Begin[3];                               // Cylinder/Head/Sector start
    uint8_t TypeCode;                                   // partition type: 0x0E-FAT16 LBA, 0x0B-FAT32, 0x0C-FAT32 LBA
    uint8_t CHS_End[3];                                 // Cylinder/Head/Sector end
    uint8_t LBA_Begin[4];                               // LBA of first sector
    uint8_t Sectors[4];                                 // numbers of sectors in partition
  } __attribute__((packed)) s_PE;

  // Master Boot Record MBR
  typedef struct s_MBR {
    uint8_t bootstrap[446];
    s_PE Partition1;                                    // 16 bytes Partition Entry
    s_PE Partition2;                                    // 16 bytes Partition Entry
    s_PE Partition3;                                    // 16 bytes Partition Entry
    s_PE Partition4;                                    // 16 bytes Partition Entry
    uint8_t Signature[2];                               // signature => must be 0xAA55
  } __attribute__((packed)) s_MBR;

  // Boot Sector (or Volume ID, or Volume Boot Sector)
  // @source https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html
  // The first sector (512 bytes) of a FAT filesystem is the boot sector. 
  // In Unix-like terminology this would be called the superblock. It contains some general information.
  typedef struct s_BS {
    uint8_t Jump[3];                                    // Boot strap short or near jump
    uint8_t OEM_Identifier[8];                          // Name - can be used to special case partition manager volumes
    uint8_t BytesPerSector[2];                          // bytes per logical sector
    uint8_t SectorsPerCluster;                          // sectors/cluster
    uint8_t ReservedSectors[2];                         // reserved sectors
    uint8_t NumberOfFATs;                               // number of FATs
    uint8_t RootEntries[2];                             // root directory entries, 0 for FAT32
    uint8_t NumberOfSectors[2];                         // number of sectors
    uint8_t MediaDescriptor;                            // media code
    uint8_t SectorsPerFAT[2];                           // sectors/FAT, 0 for FAT32
    uint8_t SectorsPerHead[2];                          // sectors per track
    uint8_t HeadsPerCylinder[2];                        // number of heads
    uint8_t HiddenSectors[4];                           // hidden sectors (unused)
    uint8_t BigNumberOfSectors[4];                      // number of sectors (if NumberOfSectors == 0)
    uint8_t BigSectorsPerFAT[4];                        // sectors/FAT
    uint8_t ExtFlags[2];                                // bit 8: fat mirroring, low 4: active fat
    uint8_t FSVersion[2];                               // major, minor filesystem version
    uint8_t RootDirectoryStart[4];                      // first cluster in root directory
    uint8_t FSInfoSector[2];                            // filesystem info sector
    uint8_t BackupBootSector[2];                        // backup boot sector
    uint8_t Reserved[12];                               // Unused
    uint8_t Empty[446];
    uint8_t Signature[2];                               // offset 0x1FE - signature => must be 0xAA55
  } __attribute__((packed)) s_BS;

  // Directory Table DT
  typedef struct s_PE {
    uint8_t Name[8];                                     // Boot indicator bit flag: 0 = no, 0x80 = bootable (or "active")
    uint8_t Extension[3];                                // Cylinder/Head/Sector start
    uint8_t Attribute;                                   // partition type: 0x0E-FAT16 LBA, 0x0B-FAT32, 0x0C-FAT32 LBA
    uint8_t CHS_End[3];                                  // Cylinder/Head/Sector end
    uint8_t LBA_Begin[4];                                // LBA of first sector
    uint8_t Sectors[4];                                 // numbers of sectors in partition
  } __attribute__((packed)) s_PE;

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
  uint16_t FAT32_Get_2Bytes_LE (uint8_t *);
  
  /**
   * @brief   Get 4 Bytes Little Endian
   *
   * @param   uint8_t * number
   *
   * @return  uint32_t
   */
  uint32_t FAT32_Get_4Bytes_LE (uint8_t *);

#endif
