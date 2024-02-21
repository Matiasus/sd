/**
 * --------------------------------------------------------------------------------------+
 * @brief       VS1053 Driver (VLSI company)
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        19.10.2022
 * @file        vs1053.h
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend      avr/io.h, util/delay.h, spi.h, lcd/ssd1306.h, fat32.h
 * --------------------------------------------------------------------------------------+
 * @interface   SPI connected through 7 pins
 * @pins        5V, DGND, MOSI, DREQ,  XCS
 *              5V, MISO,  SCK, XRST, XDCS
 *
 * @sources     https://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf
 *              https://www.vlsi.fi/fileadmin/app_notes/vs10XXan_spi.pdf
 *              https://www.vlsi.fi/player_vs1011_1002_1003/modularplayer/files.html
 *              https://github.com/adafruit/Adafruit_VS1053_Library
 *              https://os.mbed.com/users/silis/code/VS1053//file/5ad25d480d5f/VS1053.cpp/
 */

#ifndef __VS1053_H__
#define __VS1053_H__

  // INCLUDE libraries
  #include <avr/io.h>
  #include <util/delay.h>
  #include "spi.h"
  #include "lcd/ssd1306.h"

  // SPI PORT PIN Definition
  #define VS1053_DDR              SPI_DDR
  #define VS1053_PORT             SPI_PORT
  #define VS1053_SCK              SPI_SCK
  #define VS1053_MOSI             SPI_MOSI
  #define VS1053_MISO             SPI_MISO

  // RESET
  #define VS1053_DDR_XRES         DDRB
  #define VS1053_PORT_XRES        PORTB
  #define VS1053_XRES             0

  // XDCS
  #define VS1053_DDR_XDCS         DDRD
  #define VS1053_PORT_XDCS        PORTD
  #define VS1053_XDCS             7

  // XCS
  #define VS1053_DDR_XCS          DDRD
  #define VS1053_PORT_XCS         PORTD
  #define VS1053_XCS              6

  // DREQ
  #define VS1053_DDR_DREQ         DDRD
  #define VS1053_PORT_DREQ        PORTD
  #define VS1053_DREQ             2

  // REGISTERS
  // ---------------------------------------------------------------------------------------
  #define SCI_MODE                0x0 // Mode control
  #define SCI_STATUS              0x1 // Status of VS1053b
  #define SCI_BASS                0x2 // Built-in bass/treble control
  #define SCI_CLOCKF              0x3 // Clock freq + multiplier
  #define SCI_DECODE_TIME         0x4 // Decode time in seconds
  #define SCI_AUDATA              0x5 // Misc. audio data
  #define SCI_WRAM                0x6 // RAM write/read
  #define SCI_WRAMADDR            0x7 // Base address for RAM write/read
  #define SCI_HDAT0               0x8 // Stream header data 0
  #define SCI_HDAT1               0x9 // Stream header data 1
  #define SCI_AIADDR              0xA // Start address of application
  #define SCI_VOL                 0xB // Volume control
  #define SCI_AICTRL0             0xC // Application control register 0
  #define SCI_AICTRL1             0xD // Application control register 1
  #define SCI_AICTRL2             0xE // Application control register 2
  #define SCI_AICTRL3             0xF // Application control register 3

  // SCI_MODE
  // ---------------------------------------------------------------------------------------
  // SCI_MODE is used to control the operation of VS10XX and defaults to 0x4800 (SM_SDINEW set).
  #define SM_DIFF                 0x0001 // Differential [0 normal in-phase audio; 1 left channel inverted]
  #define SM_LAYER12              0x0002 // Allow MPEG layers I & II [0 no; 1 yes]
  #define SM_RESET                0x0004 // Soft reset [0 no reset; 1 reset]
  #define SM_CANCEL               0x0008 // Cancel decoding current file [0 no; 1 yes]
  #define SM_EARSPEAKER_LO        0x0010 // EarSpeaker low setting [0 off; 1 active]
  #define SM_TESTS                0x0020 // Allow SDI tests [0 allow; 1 not allow]
  #define SM_STREAM               0x0040 // Stream mode [0 no; 1 yes]
  #define SM_EARSPEAKER_HI        0x0080 // EarSpeaker high setting [0 off; 1 active]
  #define SM_DACT                 0x0100 // DCLK active edge [0 rising; 1 falling]
  #define SM_SDIORD               0x0200 // SDI bit order [0 MSb first; 1 MSb last]
  #define SM_SDISHARE             0x0400 // Share SPI chip select [0 no; 1 yes]
  #define SM_SDINEW               0x0800 // VS10xx native SPI modes [0 no; 1 yes]
  #define SM_ADPCM                0x1000 // PCM/ADPCM recording active [0 no; 1 yes]
  #define SM_LINE1                0x4000 // MIC / LINE1 selector [0 MICP; 1 LINE1]
  #define SM_CLK_RANGE            0x8000 // Input clock range [0 12..13MHz; 1 24..26MHz]
 
  // GPIO
  // --------------------------------------------------------------------------------------- 
  #define GPIO_DDR                0xC017
  #define GPIO_ODATA              0xC019

  // SCI_STATUS
  // ---------------------------------------------------------------------------------------
  // SCI_STATUS contains information on the current status of VS1053b. It also controls some
  // low-level things that the user does not usually have to care about
  #define SS_DO_NOT_JUMP          15  // Header in decode, do not fast forward/rewind
//  #define SS_SWING            14:12  // Set swing to +0 dB, +0.5 dB, .., or +3.5 dB
  #define SS_VCM_OVERLOAD         11  // GBUF overload indicator '1' = overload
  #define SS_VCM_DISABLE          10  // GBUF overload detection '1' = disable
//  #define SS_VER                7:4  // Version
  #define SS_APDOWN2              3  //
  #define SS_APDOWN1              2  //
  #define SS_AD_CLOCK             1  //
  #define SS_REFERENCE_SEL        0  //

  // READ / WRITE
  #define VS10XX_READ             0x3
  #define VS10XX_WRITE            0x2
  // Frequency
  #define VS10XX_FREQ_1kHz        0x44
  #define VS10XX_FREQ_5kHz        0x54
  // Settings
  #define VS10XX_CLOCKF_SET       0x8800
  #define VS10XX_ADDR_ENDBYTE     0x1E06
  // Memory test ok
  #define VS1003_MEMTEST_OK       0x807f
  #define VS1053_MEMTEST_OK       0x83ff

  /**
   * +-----------------------------------------------------------------------------------+
   * |== COMMUNICATION FUNCTIONS ========================================================|
   * +-----------------------------------------------------------------------------------+
   */

  /**
   * @brief   Write Serial Command Instruction
   *
   * @param   uint8_t addr
   * @param   uint16_t command
   *
   * @return  void
   */
  void VS1053_WriteSci (uint8_t, uint16_t);

  /**
   * @brief   Read Serial Command Instruction
   *
   * @param   uint8_t addr
   *
   * @return  uint16_t
   */
  uint16_t VS1053_ReadSci (uint8_t);

  /**
   * @brief   Write Serial Data / Array
   *
   * @param   uint8_t * data
   * @param   uint16_t n-times
   *
   * @return  void
   */
  void VS1053_WriteSdi (const uint8_t *, uint16_t);

  /**
   * @brief   Write Serial Data / Byte n-times
   *
   * @param   const uint8_t byte
   * @param   uint16_t n-times
   *
   * @return  void
   */
  void VS1053_WriteSdiByte (uint8_t, uint16_t);
  
  /**
   * @brief   Write To RAM
   *
   * @param   uint16_t addr
   * @param   uint16_t data
   *
   * @return  void
   */
  void VS1053_Write_To_WRAM (uint16_t, uint16_t);

  /**
   * +-----------------------------------------------------------------------------------+
   * |== TEST FUNCTIONS =================================================================|
   * +-----------------------------------------------------------------------------------+
   */

  /**
   * @brief   Test Serial Command Instruction
   *
   * @param   void
   *
   * @return  void
   */
  void VS1053_TestSci (void);

  /**
   * @brief   Sine Test
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void VS1053_TestSine (uint8_t);

  /**
   * @brief   Memory Test
   *
   * @param   void
   *
   * @return  uint16_t
   */
  uint16_t VS1053_TestMemory (void);

  /**
   * @brief   Sample Hello Test
   *
   * @param   const char *
   *
   * @return  uint16_t
   */
  uint16_t VS1053_TestSample (const char *, uint16_t);

  /**
   * +-----------------------------------------------------------------------------------+
   * |== CONTROL FUNCTIONS ==============================================================|
   * +-----------------------------------------------------------------------------------+
   */

  /**
   * @brief   Init
   *
   * @param   void
   *
   * @return  void
   */
  void VS1053_Init (void);

  /**
   * @brief   Hard reset
   * @source  https://www.vlsi.fi/player_vs1011_1002_1003/modularplayer/vs10xx_8c.html#a3
   *
   * @param   void
   *
   * @return  void
   */
  void VS1053_Reset (void);

  /**
   * @brief   Soft reset
   * @source  https://www.vlsi.fi/player_vs1011_1002_1003/modularplayer/vs10xx_8c.html#a2
   *
   * @param   void
   *
   * @return  void
   */
  void VS1053_SoftReset (void);

  /**
   * @brief   Get Version
   *
   * @param   void
   *
   * @return  char *
   */
  char * VS1053_GetVersion (void);

  /**
   * @brief   Playback Cancel
   *
   * @param   void
   *
   * @return  uint16_t
   */
  uint16_t VS1053_PlayCancel (void);

  /**
   * @brief   Set volume
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  void
   */
  void VS1053_SetVolume (uint8_t, uint8_t);

  /**
   * @brief   Send buffer to codec
   *
   * @param   uint8_t * - buffer
   * @param   uint16_t - how many times
   *
   * @return  void
   */
  void VS1053_Send_Buffer (uint8_t *, uint16_t);

  /**
   * @brief   Playing song according to order file number
   *
   * @param   FAT32_t * FAT32 structure
   * @param   uint16_t filenum - which file in order of root directory
   *
   * @return  void
   */
  void VS1053_Play_Song (FAT32_t *, uint16_t);
  
  /**
   * @brief   Switch to MP3
   * @source  https://github.com/baldram/ESP_VS1053_Library/blob/master/src/VS1053.cpp#L322
   *
   * @param   void
   *
   * @return  void
   */
  void VS1053_Switch_To_MP3 (void);

#endif
