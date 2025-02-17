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
#include "src/ui/ui.h"

/**
 * @desc    Main function
 *
 * @param   Void
 *
 * @return  int
 */
int main (void)
{
  uint8_t r;
  FAT32_t FAT32;                                          // declaration & init in fat32.c

  UI_Files_t UI_Files = {
    .Position = 1, 
    .Group = 4
  };

  // Init UI / SD Card, FAT32, LCD SSD1306
  // --------------------------------------------------------------------------------------
  if (UI_ERROR == (r = UI_Init(&FAT32))) {
    UI_Print_Error(r);
    return UI_ERROR;
  }

  UI_Clear_Screen();
  UI_Print_Frame();
  UI_Show_Song(&FAT32, 9, &UI_Files);

  // EXIT
  // ----------------------------------------------------------------
  return 0;
}