/**
 * --------------------------------------------------------------------------------------+
 * @brief       VS1053 Status Info
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        17.11.2022
 * @file        vs1053_info.h
 * @version     1.0.0
 * @test        AVR Atmega328p
 *
 * @depend      
 * --------------------------------------------------------------------------------------+
 * @interface   
 * @pins        
 *
 * @sources     https://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf
 */

#ifndef __VS1053_INFO_H__
#define __VS1053_INFO_H__

  // @includes
  #include <avr/pgmspace.h>

  #define VERS_TEXT_LEN               6             // version string length definition

  const char ver_0[] PROGMEM = "VS1001"; 
  const char ver_1[] PROGMEM = "VS1011"; 
  const char ver_2[] PROGMEM = "VS1002"; 
  const char ver_3[] PROGMEM = "VS1003"; 
  const char ver_4[] PROGMEM = "VS1053"; 
  const char ver_5[] PROGMEM = "VS1033"; 
  const char ver_6[] PROGMEM = "VS1063"; 
  const char ver_7[] PROGMEM = "VS1103";

  const char * const vs10xx_vers[] PROGMEM = {
    ver_0, 
    ver_1,
    ver_2, 
    ver_3,
    ver_4, 
    ver_5,
    ver_6, 
    ver_7,
  };

#endif
