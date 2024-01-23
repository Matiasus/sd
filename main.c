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
  uint8_t buffer[512];
  uint32_t lba_begin;
  uint32_t root_dir_clus_no;
  
  // Init LCD SSD1306
  // -------------------------------------------------------------------------------------
  SSD1306_Init (SSD1306_ADDR);
  SSD1306_ClearScreen ();
  SSD1306_SetPosition (10, 0);
  SSD1306_DrawString ("FAT32 INTERFACING", NORMAL);

  // Read MBR
  // ----------------------------------------------------------------
  if (FAT32_Init() == FAT32_ERROR) {
    SSD1306_SetPosition (20, 3);
    SSD1306_DrawString ("ERROR", NORMAL);
    return 0;
  }

  lba_begin = FAT32_Master_Boot_Record ();
  root_directory = FAT32_Boot_Sector (lba_begin);
  
  // Print
  // ----------------------------------------------------------------
  SSD1306_SetPosition (2, 2);
  SSD1306_DrawString ("LBA:  0x", NORMAL);
  sprintf (str, "%08x", (unsigned int) lba_begin);
  SSD1306_DrawString (str, NORMAL);
  
  SSD1306_SetPosition (2, 3);
  SSD1306_DrawString ("ROOT: 0x", NORMAL);
  sprintf (str, "%08x", (unsigned int) root_directory);
  SSD1306_DrawString (str, NORMAL);

  // EXIT
  // -------------------------------------------------------------------------------------
  return 0;
}
