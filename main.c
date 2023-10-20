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
  SPI_PortInit ();
  SPI_SlowSpeedInit ();

  // init LCD SSD1306
  // -------------------------------------------------------------------------------------
  SSD1306_Init ();                                                // init lcd
  SSD1306_ClearScreen ();
  SSD1306_SetPosition (10, 0);
  SSD1306_DrawString ("SDCARD INTERFACING", NORMAL);

  // init SD Card
  // -------------------------------------------------------------------------------------
  SSD1306_SetPosition (1, 2);
  SSD1306_DrawString ("SD Card init", NORMAL);
  SSD1306_SetPosition (103, 2); 
  if (SD_Init () == SD_SUCCESS) {
    SSD1306_DrawString ("[ok]", NORMAL);
  } else {
    SSD1306_DrawString ("[ko]", NORMAL);
  }

  // EXIT
  // -------------------------------------------------------------------------------------
  return 0;
}
