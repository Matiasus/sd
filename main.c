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
  char str[10];
  //
  // -------------------------------------------------------------------------------------  
  SD sd = { .success = 0, .cmd8_voltage_accept = 0, .cmd58_sdhc = 0 };

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
  if (SD_Init (&sd) == SD_SUCCESS) {
    SSD1306_DrawString ("[ok]", NORMAL);
  } else {
    SSD1306_DrawString ("[ko]", NORMAL);
  }
  SSD1306_SetPosition (1, 4);   
  sprintf (str, "SD: %x %x %x", sd.success, sd.cmd8_voltage_accept, sd.cmd58_sdhc);
  SSD1306_DrawString (str, NORMAL);
  
  // EXIT
  // -------------------------------------------------------------------------------------
  return 0;
}
