/**
 * --------------------------------------------------------------------------------------+
 * @brief       SAY HELLO / SD / uSD CARD
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        24.09.2023
 * @file        main.c
 * @version     1.0.0
 * @test        AVR Atmega328p
 *
 * @depend      src/sd.h
 * --------------------------------------------------------------------------------------+
 * @interface   SPI
 * @pins
 *
 * @sources
 */

// INCLUDE libraries
#include "src/fat32.h"
#include "src/vs1053/vs1053.h"
//#include "src/vs1053/vs1053_hello.h"

/**
 * @desc    Main function
 *
 * @param   Void
 *
 * @return  int
 */
int main (void)
{
  char str[10];

  uint32_t files;
  FAT32_t FAT32 = {
    .root_dir_clus_num = 0, 
    .sectors_per_cluster = 0, 
    .lba_begin = 0, 
    .fat_area_begin = 0, 
    .data_area_begin = 0, 
  };

  // Init LCD SSD1306
  // ----------------------------------------------------------------
  SSD1306_Init (SSD1306_ADDR);
  SSD1306_ClearScreen ();
  SSD1306_SetPosition (7, 0);
  SSD1306_DrawString ("MP3 SD-FAT32 PLAYER", NORMAL);

  // Init SD
  // ----------------------------------------------------------------
  SSD1306_SetPosition (1, 4);
  SSD1306_DrawString ("FAT32", NORMAL);
  SSD1306_SetPosition (103, 4);
  if (FAT32_Init(&FAT32) == FAT32_ERROR) {
    SSD1306_DrawString ("[ER]", NORMAL);
    return FAT32_ERROR;
  }
  SSD1306_DrawString ("[OK]", NORMAL);

  // Root Directory Number Of Files
  // ----------------------------------------------------------------
  /*
  files = FAT32_Root_Dir_Files (&FAT32);
  SSD1306_SetPosition (1, 5);
  (files > 10) ? (sprintf (str, "[%d]", (int) files)) : (sprintf (str, "[ %d]", (int) files));
  SSD1306_DrawString ("FILES", NORMAL);
  SSD1306_SetPosition (103, 5);
  SSD1306_DrawString (str, NORMAL);
*/
  // VS1053 Init
  // ----------------------------------------------------------------
  VS1053_Init ();
/*
  // VS1053 Memory Test
  // ----------------------------------------------------------------
  SSD1306_SetPosition (1, 3);
  SSD1306_DrawString ("CODEC", NORMAL);
  SSD1306_SetPosition (103, 3);
  data = VS1053_TestMemory ();   
  if (data != VS1053_MEMTEST_OK) {
    SSD1306_DrawString ("[ER]", NORMAL);
    return FAT32_ERROR;
  }
  VS1053_TestSine (VS10XX_FREQ_1kHz);                             // sine test 1kHz
  //VS1053_TestSine (VS10XX_FREQ_5kHz);                             // sine test 5kHz
  VS1053_TestSample (HelloMP3, sizeof(HelloMP3)-1);               // say Hello
  SSD1306_DrawString ("[OK]", NORMAL);
*/
  // Read File
  // ----------------------------------------------------------------
  SSD1306_SetPosition (1, 5);
  SSD1306_DrawString ("PLAY", NORMAL);
  SSD1306_SetPosition (103, 5);
  SSD1306_DrawString ("[ 3]", NORMAL);
  VS1053_Play_Song_Test (&FAT32, 1);

  // EXIT
  // ----------------------------------------------------------------
  return 0;
}