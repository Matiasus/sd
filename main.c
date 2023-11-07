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

/**
 * @desc    Main function
 *
 * @param   Void
 *
 * @return  int
 */
int main (void)
{
  // SD init structure
  // -------------------------------------------------------------------------------------
  SD sd = {
    .voltage = 0,
    .sdhc = 0,
    .version = 0
  };

  // init LCD SSD1306
  // -------------------------------------------------------------------------------------
  SSD1306_Init ();                                                // init lcd
  SSD1306_ClearScreen ();
  SSD1306_SetPosition (10, 0);
  SSD1306_DrawString ("SDCARD INTERFACING", NORMAL);

  // init SD Card
  // -------------------------------------------------------------------------------------
  if (SD_Init (&sd) == SD_SUCCESS) {
    if (sd.voltage == 1) {
      SSD1306_SetPosition (1, 2);
      SSD1306_DrawString ("VOLT: 2.7-3.6V", NORMAL);
    }
    if (sd.sdhc == 1) {
      SSD1306_SetPosition (1, 3);
      SSD1306_DrawString ("SDSC: <2GB", NORMAL);
    } else if (sd.sdhc == 2) {
      SSD1306_SetPosition (1, 3);
      SSD1306_DrawString ("SDHC: 2-32GB", NORMAL);     
    }
    if ((sd.version == 1) || (sd.version == 2)) {
      SSD1306_SetPosition (1, 4);
      SSD1306_DrawString ("VERS: 2", NORMAL);
    }
  } else {
    SSD1306_SetPosition (1, 2);
    SSD1306_DrawString ("SD ERROR", NORMAL);
  }

  // EXIT
  // -------------------------------------------------------------------------------------
  return 0;
}
