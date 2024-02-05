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
  FAT32_t FAT32 = {.lba_begin =0, .fats_begin = 0, .data_begin = 0, .root_begin = 0};
  
  // Init LCD SSD1306
  // -------------------------------------------------------------------------------------
  SSD1306_Init (SSD1306_ADDR);
  SSD1306_ClearScreen ();
  SSD1306_SetPosition (10, 0);
  SSD1306_DrawString ("FAT32 INTERFACING", NORMAL);

  // Init SD
  // ----------------------------------------------------------------
  if (FAT32_Init() == FAT32_ERROR) {
    SSD1306_SetPosition (20, 3);
    SSD1306_DrawString ("ERROR", NORMAL);
    return FAT32_ERROR;
  }

  // Read Master Boot Record
  // ----------------------------------------------------------------
  FAT32_Read_Master_Boot_Record (&FAT32);

  // Read Boot Sector
  // ----------------------------------------------------------------
  FAT32_Read_Boot_Sector (&FAT32);

  // Rood Directory Number Of Files
  // ---------------------------------------------------------------- 
  SSD1306_SetPosition (0, 2);
  sprintf (str, "%ld", FAT32_Root_Dir_Files (&FAT32));
  SSD1306_DrawString ("Files: ", NORMAL);
  SSD1306_DrawString (str, NORMAL);

  // EXIT
  // ----------------------------------------------------------------
  return 0;
}
