/**
 * --------------------------------------------------------------------------------------+
 * @brief       VS1053 Driver (VLSI company)
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        19.10.2022
 * @file        vs1053.c
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend      vs1053.h, vs1053_info.h
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

// INCLUDE libraries
#include "../fat32.h"
#include "vs1053.h"
#include "vs1053_info.h"

// global variables
char buffer[VERS_TEXT_LEN];

/**
 * +------------------------------------------------------------------------------------+
 * |== STATIC FUNCTIONS ================================================================|
 * +------------------------------------------------------------------------------------+
 */

/* Activate Command / clear XCS */
static inline void VS1053_ActivateCommand (void) { VS1053_PORT_XCS &= ~(1 << VS1053_XCS); }
/* Deactivate Command / set XCS */
static inline void VS1053_DeactivateCommand (void) { VS1053_PORT_XCS |= (1 << VS1053_XCS); }

/* Activate Data / clear XDCS */
static inline void VS1053_ActivateData (void) { VS1053_PORT_XDCS &= ~(1 << VS1053_XDCS); }
/* Deactivate Data / set XDCS */
static inline void VS1053_DeactivateData (void) { VS1053_PORT_XDCS |= (1 << VS1053_XDCS); }

/* Activate RESET / clear XRST */
static inline void VS1053_ActivateReset (void) { VS1053_PORT_XRES &= ~(1 << VS1053_XRES); }
/* Deactivate RESET / set XRST */
static inline void VS1053_DeactivateReset (void) { VS1053_PORT_XRES |= (1 << VS1053_XRES); }

/* DREQ Wait */
static inline void VS1053_DreqWait (void) { while (!(VS1053_PORT_DREQ & (1 << VS1053_DREQ))); }

/**
 * +-----------------------------------------------------------------------------------+
 * |== COMMUNICATION FUNCTIONS ========================================================|
 * +-----------------------------------------------------------------------------------+
 */

/**
 * @brief   Write Serial Command Instruction / big endian /
 *
 * @param   uint8_t addr
 * @param   uint16_t command
 *
 * @return  void
 */
void VS1053_WriteSci (uint8_t addr, uint16_t command)
{
  VS1053_DreqWait ();                                   // wait until DREQ is high
  VS1053_ActivateCommand ();                            // clear xCS
  SPI_Transfer (VS10XX_WRITE);                          // command code for WRITE
  SPI_Transfer (addr);                                  // SCI register number
  SPI_Transfer ((uint8_t)(command >> 8));               // high byte
  SPI_Transfer ((uint8_t)(command & 0xFF));             // low byte
  VS1053_DeactivateCommand ();                          // set xCS
}

/**
 * @brief   Read Serial Command Instruction / big endian /
 *
 * @param   uint8_t addr
 *
 * @return  uint16_t
 */
uint16_t VS1053_ReadSci (uint8_t addr)
{
  uint16_t data;

  VS1053_DreqWait ();                                   // wait until DREQ is high
  VS1053_ActivateCommand ();                            // clear xCS
  SPI_Transfer (VS10XX_READ);                           // command code for READ
  SPI_Transfer (addr);                                  // SCI register number
  data = (uint16_t) SPI_Transfer (0x00) << 8;           // high byte
  data |= SPI_Transfer (0x00);                          // low byte
  VS1053_DeactivateCommand ();                          // set xCS

  return data;                                          // return content
}

/**
 * @brief   Write Serial Data / Array
 *
 * @param   const uint8_t * data
 * @param   uint16_t n-times
 *
 * @return  void
 */
void VS1053_WriteSdi (const uint8_t * data, uint16_t n)
{
  uint8_t i;
  uint16_t length;

  while (n) {
    length = (n > 32) ? 32 : n;                         // max 32
    VS1053_DreqWait ();                                 // wait until DREQ is high
    VS1053_ActivateData ();                             // clear xDCS
    for (i = 0; i < length; i++) {
      SPI_Transfer (*data++);                           // send data
      n--;                                              // decrement
    }
    VS1053_DeactivateData ();                           // set xDCS
  }
  VS1053_DreqWait ();                                   // wait until DREQ is high
}

/**
 * @brief   Write Serial Data / Byte n-times
 *
 * @param   uint8_t byte
 * @param   uint16_t n-times
 *
 * @return  void
 */
void VS1053_WriteSdiByte (uint8_t byte, uint16_t n)
{
  uint8_t i;
  uint16_t length;

  while (n) {
    length = (n > 32) ? 32 : n;                         // max 32
    VS1053_DreqWait ();                                 // wait until DREQ is high
    VS1053_ActivateData ();                             // clear xDCS
    for (i = 0; i < length; i++) {                      // send data
      SPI_Transfer (byte);
      n--;
    }
    VS1053_DeactivateData ();                           // set xDCS
  }
  VS1053_DreqWait ();                                   // wait until DREQ is high
}

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
void VS1053_TestSci (void)
{
  while (1)
  {
    VS1053_WriteSci (SCI_VOL, 0x0000);                  // set full volume
    _delay_ms (500);                                    // delay

    VS1053_WriteSci (SCI_VOL, 0xFFFF);                  // activate analog powerdown mode
    _delay_ms (500);                                    // delay
  }
}

/**
 * @brief   Sine Test
 *          https://www.vlsi.fi/player_vs1011_1002_1003/modularplayer/vs10xx_8c-source.html
 *
 * @param   uint8_t
 *
 * @return  void
 */
void VS1053_TestSine (uint8_t n)
{
  // Fsinetest = Fs * S/128

  // +----------------------------------+
  // |             n bits               |
  // +----------------------------------+
  // | NAME  | BITS | DESCRIPTION       |
  // +----------------------------------+
  // | FsIdx | 7:5  | Sample rate index |
  // +----------------------------------+
  // | S     | 4:0  | Sine skip speed   |
  // +----------------------------------+
  //
  // +----------------------------------+
  // |   FsIdx  |          Fs           |
  // +----------------------------------+
  // | b7 b6 b5 |                       |
  // +----------------------------------+
  //   0  0  0  |       44100 Hz        |
  //   0  0  1  |       48000 Hz        |
  //   0  1  0  |       32000 Hz        |
  //   0  1  1  |       22050 Hz        |
  //   1  0  0  |       24000 Hz        |
  //   1  0  1  |       16000 Hz        |
  //   1  1  0  |       11025 Hz        |
  //   1  1  1  |       12000 Hz        |
  // +----------------------------------+
  //
  // EXAMPLE: 1kHz
  // ------------------------------------
  // Fs = 32000; S = F * 128 / Fs = 1000 * 128 / 32000 = 4
  // 32000Hz => FsIdx = 2; S = 4; n = 0b0100 0100 = 0x44
  //
  // EXAMPLE: 5kHz
  // ------------------------------------
  // Fs = 32000; S = F * 128 / Fs = 5000 * 128 / 32000 = 20
  // 32000Hz => FsIdx = 2; S = 20; n = 0b0101 0100 = 0x54
  uint16_t sci_mode;
  uint8_t sine_activate[] = {0x53, 0xEF, 0x6E, n, 0, 0, 0, 0};
  const uint8_t sine_deactivate[] = {0x45, 0x78, 0x69, 0x74, 0, 0, 0, 0};

  // hardware reset 
  // ----------------------------------------------------------------------------------  
  VS1053_Reset ();                                      // hardware reset

  // test mode setting
  // ----------------------------------------------------------------------------------
  sci_mode = VS1053_ReadSci (SCI_MODE);                 // read MODE register
  VS1053_WriteSci (SCI_MODE, sci_mode | SM_TESTS);      // SM_SDINEW | SM_TESTS

  // sine wave sequence start
  // ----------------------------------------------------------------------------------
  VS1053_WriteSdi (sine_activate, 8);                   // Sine wave data activate
  _delay_ms (1000);                                     // delay

  // sine wave sequence stop
  // ----------------------------------------------------------------------------------
  VS1053_WriteSdi (sine_deactivate, 8);                 // Sine wave data deactivate
  _delay_ms (100);                                      // delay
}

/**
 * @brief   Memory Test
 *
 * @param   void
 *
 * @return  uint16_t
 */
uint16_t VS1053_TestMemory (void)
{
  uint16_t data;
  uint16_t sci_mode;
  const uint8_t mem_sequence[] = {0x4D, 0xEA, 0x6D, 0x54, 0, 0, 0, 0};

  // hardware reset 
  // ----------------------------------------------------------------------------------  
  //VS1053_Reset ();                                      // hardware reset  

  // test mode setting
  // ----------------------------------------------------------------------------------  
  sci_mode = VS1053_ReadSci (SCI_MODE);                 // read MODE register
  VS1053_WriteSci (SCI_MODE, sci_mode | SM_TESTS);      // SM_SDINEW | SM_TESTS

  // test memory sequence
  // ---------------------------------------------------------------------------------- 
  VS1053_WriteSdi (mem_sequence, 8);                    // memory test sequence
  _delay_ms (300);                                      // wait for 1100000 clock cycles
  data = VS1053_ReadSci (SCI_HDAT0);                    // result read from the SCI reg SCI_HDAT0

  VS1053_SoftReset ();                                  // soft reset

  return data;
}

/**
 * @brief   Hello Test
 *
 * @param   const char *
 * @param   uint16_t
 *
 * @return  uint16_t
 */
uint16_t VS1053_TestSample (const char * sample, uint16_t n)
{
  uint16_t i = 0;

  // hardware reset 
  // ----------------------------------------------------------------------------------  
  //VS1053_Reset ();                                      // hardware reset    

  while (i < n) {
    while (!(VS1053_PORT_DREQ & (1 << VS1053_DREQ))) {       // DREQ wait
      VS1053_DeactivateData ();                         // set xDCS
    }
    VS1053_ActivateData ();                             // clear xDCS
    SPI_Transfer (pgm_read_byte(&sample[i++]));         // send data
  }
  _delay_ms (100);
  
  // Cancel playback
  // ----------------------------------------------------------------------------------
  return VS1053_PlayCancel ();
}

/**
 * @brief   Send buffer to codec
 *
 * @param   uint8_t * - buffer
 * @param   uint16_t - how many times
 *
 * @return  void
 */
void VS1053_Send_Buffer (uint8_t * buffer, uint16_t n)
{
  uint16_t i = 0;

  while (i < n) {
    while (!(VS1053_PORT_DREQ & (1 << VS1053_DREQ))) {  // DREQ wait
      VS1053_DeactivateData ();                         // set xDCS
    }
    VS1053_ActivateData ();                             // clear xDCS
    SPI_Transfer (buffer[i++]);                         // send data
  }
}

/**
 * @brief   Playing song according to order file number
 *
 * @param   FAT32_t * - FAT32 structure
 * @param   uint16_t - which file in order of root directory
 *
 * @return  void
 */
void VS1053_Play_Song (FAT32_t * FAT32, uint16_t filenum)
{
  uint8_t sectors = FAT32->sectors_per_cluster;
  uint8_t buffer[BYTES_PER_SECTOR];
  uint32_t sector;

  DE_t * File = FAT32_Get_File_Info (FAT32, (uint32_t) filenum);
  uint32_t cluster = ((uint32_t) FAT32_Get_2Bytes_LE (File->FirstClustHI) << 16) | FAT32_Get_2Bytes_LE (File->FirstClustLO);

  // Reset
  // ----------------------------------------------------------------------------------
  //VS1053_Reset ();                                      // hardware reset
  VS1053_SoftReset ();                                  // software reset

  do {

    sector = FAT32_Get_1st_Sector_Of_Clus (FAT32, cluster);

    // Read Cluster
    // --------------------------------------------------------------------------------
    while (sectors--) {
      SD_Read_Block (sector++, buffer);                 // Read Sector
      VS1053_Send_Buffer (buffer, BYTES_PER_SECTOR);    // send buffer to codec
    }

    cluster = FAT32_FAT_Next_Cluster (FAT32, cluster);  // get next cluster
    cluster &= 0x0FFFFFFF;                              // mask first nibble

  } while (cluster < 0x0FFFFFF8);                       // 0x?ffffff8 - 0x?fffffff = Last cluster in file (EOC)
}

/**
 * @brief   Testing
 *
 * @param   FAT32_t * - FAT32 structure
 * @param   uint16_t - which file in order of root directory
 *
 * @return  void
 */
void VS1053_Play_Song_Test (FAT32_t * FAT32, uint16_t filenum)
{
  char str[10];
  uint8_t sectors = FAT32->sectors_per_cluster;
  uint8_t buffer[BYTES_PER_SECTOR];
  uint32_t sector;

  DE_t * File = FAT32_Get_File_Info (FAT32, (uint32_t) filenum);
  uint32_t cluster = ((uint32_t) FAT32_Get_2Bytes_LE (File->FirstClustHI) << 16) | FAT32_Get_2Bytes_LE (File->FirstClustLO);

  SSD1306_ClearScreen ();
  sprintf (str, "%08x ", (unsigned int) cluster);
  SSD1306_DrawString (str, NORMAL);

  do {

    sector = FAT32_Get_1st_Sector_Of_Clus (FAT32, cluster);
/*
    // Read Cluster
    // --------------------------------------------------------------------------------
    while (sectors--) {
      SD_Read_Block (sector++, buffer);                 // Read Sector

      for (uint16_t i=0; i<512; i++) {
        sprintf (str, "%02x ", (unsigned int) buffer[i]);
        SSD1306_DrawString (str, NORMAL);
      }
    }
*/
    cluster = FAT32_FAT_Next_Cluster (FAT32, cluster);  // get next cluster
    sprintf (str, "%08x ", (unsigned int) cluster);
    SSD1306_DrawString (str, NORMAL);

    cluster &= 0x0FFFFFFF;                              // mask first nibble

  } while (cluster < 0x0FFFFFF8);                       // 0x?ffffff8 - 0x?fffffff = Last cluster in file (EOC)

}

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
void VS1053_Init (void)
{
  VS1053_DDR_XRES |= (1 << VS1053_XRES);                // RESET as output
  VS1053_DDR_XDCS |= (1 << VS1053_XDCS);                // DATA SELECT as output
  VS1053_DDR_XCS |= (1 << VS1053_XCS);                  // CHIP SELECT as output

  VS1053_DDR_DREQ &= ~(1 << VS1053_DREQ);               // DATA REQUEST as input
  VS1053_PORT_DREQ |= (1 << VS1053_DREQ);               // DATA REQUEST pullup activate

  SPI_Init (SPI_MASTER |                                // Slow Speed Init
            SPI_MODE_0 | 
            SPI_MSB_FIRST | 
            SPI_FOSC_DIV_128, 0);                       // f = fclk/128 = 125 kHz
  SPI_Enable ();

  VS1053_Reset ();                                      // init reset routine
}

/**
 * @brief   Hard reset
 *          https://www.vlsi.fi/player_vs1011_1002_1003/modularplayer/vs10xx_8c.html#a3
 *
 * @param   void
 *
 * @return  void
 */
void VS1053_Reset (void)
{
  VS1053_ActivateReset ();                              // clear XRST
  _delay_ms (2);                                        // after a hardware reset (or at power-up) DREQ will stay down for around 22000 clock cycles,
                                                        // which means an approximate 1.8 ms delay if VS1053b is run at 12.288 MHz
  SPI_Transfer (0xFF);                                  // send dummy SPI byte to initialize SPI

  // Un-reset MP3 chip
  VS1053_DeactivateCommand ();                          // set xCS
  VS1053_DeactivateData ();                             // set xDCS
  VS1053_DeactivateReset ();                            // set XRST
  VS1053_SetVolume (0xff,0xff);                         // activate analog powerdown mode

  // SCI_CLOCKF register
  // ---------------------------------------
  //
  // EXAMPLE: 0x9CCC
  // ---------------------------------------
  // 8 4 2 1 | 8 4 2 1 | 8 4 2 1 | 8 4 2 1
  // 1 0 0 1   1 1 0 0   1 1 0 0   1 1 0 0
  //
  // SC_MULT = 4 (XTALI x 3.5)
  // SC_ADD  = 3 (XTALI x 2.0)
  // SC_FREQ = 0x4CC = 1228
  // XTALI = 1228*4000+8000000 = 1291200Hz
  //
  // EXAMPLE: 0x9800
  // ---------------------------------------
  // 8 4 2 1 | 8 4 2 1 | 8 4 2 1 | 8 4 2 1
  // 1 0 0 1   1 0 0 0   0 0 0 0   0 0 0 0
  //
  // SC_MULT = 4 (XTALI x 3.5)
  // SC_ADD  = 3 (XTALI x 2.0)
  // SC_FREQ = 0 then XTALI = 12.288 MHz
  //   12.288MHz * 3.5 and
  //   12.288MHz * 5.5 if more cycles are temporarily needed to decode a WMA or AAC stream
  //VS1053_WriteSci (SCI_CLOCKF, 0x9800);               // vlsi sulution 0x9CCC (XTALI=1291200Hz)
  //
  // EXAMPLE: 0x8800
  // ---------------------------------------
  // 8 4 2 1 | 8 4 2 1 | 8 4 2 1 | 8 4 2 1
  // 1 0 0 0   1 0 0 0   0 0 0 0   0 0 0 0
  //
  // SC_MULT = 4 (XTALI x 3.5)
  // SC_ADD  = 1 (XTALI x 1.0)
  // SC_FREQ = 0 then XTALI = 12.288 MHz
  //   12.288MHz * 3.5 and
  //   12.288MHz * 4.5 if more cycles are temporarily needed to decode a WMA or AAC stream
  VS1053_WriteSci (SCI_CLOCKF, VS10XX_CLOCKF_SET);      // http://www.vsdsp-forum.com/phpbb/viewtopic.php?t=65, 0x8800
  VS1053_DreqWait ();                                   // wait until DREQ is high

  VS1053_WriteSci (SCI_AUDATA, 0x000A);                 // slow sample rate for slow analog part startup 10 Hz
  _delay_ms (100);                                      // delay

  VS1053_SetVolume (0xfe,0xfe);                         // switch on the analog parts
  VS1053_WriteSci (SCI_AUDATA, 0x1F41);                 // 8kHz, mono
  VS1053_SetVolume (0x66,0x66);                         // set volume level

  VS1053_SoftReset();                                   // soft reset
  //SPI_FastSpeedInit ();                                 // f = fclk/16 * 2 = 1MHz

  SPI_Init (SPI_MASTER |                                // Fast Speed Init
            SPI_MODE_0 | 
            SPI_MSB_FIRST | 
            SPI_FOSC_DIV_16, 1);                        // f = fclk/16 = 1 MHz
  SPI_Enable ();
}

/**
 * @brief   Soft reset
 *          https://www.vlsi.fi/player_vs1011_1002_1003/modularplayer/vs10xx_8c.html#a2
 *
 * @param   void
 *
 * @return  void
 */
void VS1053_SoftReset (void)
{
  VS1053_WriteSci (SCI_MODE, SM_SDINEW | SM_RESET);     // VS10xx native SPI modes, Soft reset
  _delay_ms (1);                                        // delay
  VS1053_DreqWait ();                                   // wait until DREQ is high

  VS1053_WriteSci (SCI_CLOCKF, VS10XX_CLOCKF_SET);      // set clock cycles to 0x8800
  _delay_ms (1);                                        // delay
  VS1053_DreqWait ();                                   // wait until DREQ is high

  SPI_Transfer (0);                                     // send 4 x 0
  SPI_Transfer (0);
  SPI_Transfer (0);
  SPI_Transfer (0);
  VS1053_DeactivateData ();                             // set xDCS
}

/**
 * @brief   Get Version
 *
 * @param   void
 *
 * @return  char *
 */
char * VS1053_GetVersion (void)
{
  char * p;                                             // pointer to string
  uint8_t v;                                            // version

  v = (uint8_t) VS1053_ReadSci (SCI_STATUS);            // read lower byte
  v >>= 4;                                              // read upper nibble

  p = (char *) pgm_read_word (&vs10xx_vers[v]);         // read version value
  strcpy_P (buffer, p);                                 // copy content into buffer

  return buffer;                                        // return string
}

/**
 * @brief   Playback Cancel
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint16_t VS1053_PlayCancel (void)
{
  uint8_t i = 0;
  uint8_t n = 64;
  uint8_t endbyte;

  // read extra parameter - endFillByte
  // ----------------------------------------------------------------------------------
  VS1053_WriteSci (SCI_WRAMADDR, VS10XX_ADDR_ENDBYTE);
  endbyte = (uint8_t) VS1053_ReadSci (SCI_WRAM) & 0xff;

  // send at least 2052 bytes of endFillByte
  // ----------------------------------------------------------------------------------
  VS1053_WriteSdiByte (endbyte, 2052);
  //_delay_ms (10);                                       // accor. to BALDRAM

  // set SCI_MODE bit SM_CANCEL
  // ----------------------------------------------------------------------------------
  VS1053_WriteSci (SCI_MODE, SM_SDINEW | SM_CANCEL);

  // send at least 32 bytes of endFillByte, max 2048 bytes then read SCI_MODE.
  // If SM_CANCEL is still set, send next 32 bytes of endfillbyte
  // If SM_CANCEL hasn't cleared after sending 2048 bytes, do a software reset
  // ----------------------------------------------------------------------------------
  VS1053_ActivateData ();                               // clear xDCS
  while (n--) {
    VS1053_DreqWait ();                                 // wait until DREQ is high
    for (i = 0; i < 32; i++) {                          // send data
      SPI_Transfer (endbyte);
    }
    if (!(VS1053_ReadSci (SCI_MODE) && SM_CANCEL)) {    // bit SM_CANCEL is null?
      VS1053_DeactivateData ();                         // set xDCS
      return VS1053_ReadSci (SCI_HDAT0);                // exit
    }
    _delay_ms (10);                                     // accor. to BALDRAM
  }
  VS1053_DeactivateData ();                             // set xDCS
  VS1053_SoftReset ();                                  // software reset required

  return VS1053_ReadSci (SCI_HDAT0);                    // exit
}

/**
 * @brief   Set volume
 *
 * @param   uint8_t lef channel
 * @param   uint8_t right channel
 *
 * @return  void
 */
void VS1053_SetVolume (uint8_t left, uint8_t right)
{
  uint16_t volume = (left << 8) | right;                // set volume integer
  VS1053_WriteSci (SCI_VOL, volume);                    // send command
}
