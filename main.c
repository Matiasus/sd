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
  DE_t * File;
  uint8_t files;
  uint32_t cluster;
  FAT32_t FAT32 = {
    .root_dir_clus_num = 0, 
    .sectors_per_cluster = 0, 
    .lba_begin = 0, 
    .fats_begin = 0, 
    .data_begin = 0, 
    .root_begin = 0
  };
  
  // Init LCD SSD1306
  // -------------------------------------------------------------------------------------
  SSD1306_Init (SSD1306_ADDR);
  SSD1306_ClearScreen ();
  SSD1306_SetPosition (10, 0);
  SSD1306_DrawString ("FAT32 INTERFACING", NORMAL);

  // Init SD
  // ----------------------------------------------------------------
  if (FAT32_Init(&FAT32) == FAT32_ERROR) {
    SSD1306_SetPosition (20, 3);
    SSD1306_DrawString ("ERROR", NORMAL);
    return FAT32_ERROR;
  }

  // Rood Directory Number Of Files
  // ----------------------------------------------------------------
  files = FAT32_Root_Dir_Files (&FAT32);
  SSD1306_SetPosition (0, 2);
  sprintf (str, "%d", files);
  SSD1306_DrawString ("SUMs: ", NORMAL);
  SSD1306_DrawString (str, NORMAL);

  // Rood Directory Number Of Files
  // ----------------------------------------------------------------
  for (uint32_t i = 1; i<7; i++) {

    File = FAT32_Get_File (&FAT32, i);
    cluster = ((uint32_t) FAT32_Get_2Bytes_LE (File->FirstClustHI) << 16) | FAT32_Get_2Bytes_LE (File->FirstClustLO);

    SSD1306_SetPosition (0, 3);
    SSD1306_DrawString ("FILE: ", NORMAL);
    sprintf (str, "%d", (int) i);
    SSD1306_SetPosition (0, 4);
    SSD1306_DrawString ("NAME: ", NORMAL);
    SSD1306_DrawString ((char *) File->Name, NORMAL);
    SSD1306_SetPosition (0, 5);
    SSD1306_DrawString ("CLUS: ", NORMAL);
    sprintf (str, "0x%08x", (unsigned int) cluster);
    SSD1306_DrawString (str, NORMAL);
    
    _delay_ms (2000);
    //SSD1306_ClearScreen ();
  }

  // EXIT
  // ----------------------------------------------------------------
  return 0;
}
