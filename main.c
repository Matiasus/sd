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
#include "src/sd.h"
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
  //uint16_t j = 0;
  char str[10];
  uint8_t buffer[512];
  
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

  SD_Read_Block (0, buffer);
  s_MBR * MBR = (s_MBR *) buffer;

  SSD1306_SetPosition (2, 4);
  SSD1306_DrawString ("SIGN: 0x", NORMAL);
  sprintf (str, "%x%x", MBR->signature[0], MBR->signature[1]);
  SSD1306_DrawString (str, NORMAL);

  uint32_t lba;
  lba = ((((unsigned long) MBR->partition1.LBABegin[3])<<24) & 0xFF000000) |
        ((((unsigned long) MBR->partition1.LBABegin[2])<<16) & 0x00FF0000) |
        ((((unsigned long) MBR->partition1.LBABegin[1])<< 8) & 0x0000FF00) |
        ((((unsigned long) MBR->partition1.LBABegin[0])<< 0) & 0x000000FF) ;

  SSD1306_SetPosition (2, 5);
  SSD1306_DrawString ("LBAS: 0x", NORMAL);
  sprintf (str, "%08x", (unsigned int) lba);
  SSD1306_DrawString (str, NORMAL);

  // EXIT
  // -------------------------------------------------------------------------------------
  return 0;
}
