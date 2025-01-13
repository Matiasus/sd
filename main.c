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
  DE_t * file;
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
  SSD1306_ClearScreen ();;
  SSD1306_SetPosition (15, 0);
  SSD1306_DrawString ("SD-FAT32 LIBRARY", NORMAL);
  SSD1306_DrawLineHorizontal(0, 1, END_COLUMN_ADDR, MIDDLEDOUBLE);

  // Init SD
  // ----------------------------------------------------------------
  if (FAT32_Init(&FAT32) == FAT32_ERROR) {
    SSD1306_SetPosition (40, 4);
    SSD1306_DrawString ("[ERROR]", NORMAL);
    return FAT32_ERROR;
  }

  // Root Directory Number Of Files
  // ----------------------------------------------------------------
  files = FAT32_Root_Dir_Files (&FAT32);
  sprintf (str, "[%02d]", (int) files);
  SSD1306_SetPosition (1, 2);
  SSD1306_DrawString ("Files ", NORMAL);
  SSD1306_DrawString (str, NORMAL);

  // Info of 5th file
  // ----------------------------------------------------------------
  file = FAT32_Get_File_Info (&FAT32, 5);
  SSD1306_SetPosition (1, 3);
  SSD1306_DrawString ("5th file ", NORMAL);
  SSD1306_DrawString ((char *) file->Name, NORMAL);

  // EXIT
  // ----------------------------------------------------------------
  return 0;
}