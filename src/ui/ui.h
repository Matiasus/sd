/**
 * --------------------------------------------------------------------------------------+
 * @brief       MP3 Interface
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2024 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        14.03.2024
 * @file        mp3.h
 * @version     1.0.0
 * @test        AVR Atmega328p
 *
 * @depend      ../vs1053/vs1053.h
 * --------------------------------------------------------------------------------------+
 * @interface   
 * @pins
 *
 * @sources
 */

#ifndef __UI_H__
#define __UI_H__

  // INCLUDE libraries
  // ------------------------------------------------------------------
  #include "../fat32/fat32.h"
  #include "../lcd/ssd1306.h"

  // RETURN
  // ------------------------------------------------------------------
  #define UI_ERROR               0x05
  #define UI_SUCCESS             0x00

  #define UI_FRAME_MARGIN    3

  // Files
  // --------------------------------------------------------------------------------------
  typedef struct UI_Files_t {
    uint8_t Position;
    uint8_t Page;
    uint8_t Page_position;
    uint8_t Count;
    uint8_t Group;
    uint8_t Pages;
  } UI_Files_t;


  /**
   * @brief   SD Card Init
   *
   * @param   FAT32_t *
   *
   * @return  uint8_t
   */
  uint8_t UI_Init(FAT32_t *);

  /**
   * @brief   Mp3 LCD Play & Show Song
   *
   * @param   FAT32_t * FAT32
   * @param   uint8_t song
   * @param   UI_Files_t * UI_files struct
   *
   * @return  void
   */
  void UI_Show_Song(FAT32_t *, uint8_t, UI_Files_t *);

  /**
   * @brief   Mp3 LCD Clear Screen
   *
   * @param   void
   *
   * @return  void
   */
  void UI_Clear_Screen(void);

  /**
   * @brief   Mp3 LCD Clear Pages From definded position to end
   *
   * @param   uint8_t from page
   * @param   uint8_t to page
   * @param   uint8_t margin
   *
   * @return  void
   */
  void UI_Clear_Pages(uint8_t, uint8_t, uint8_t);

  /**
   * @brief   Mp3 LCD Print String
   *
   * @param   char *
   * @param   E_Font
   *
   * @return  void
   */
  void UI_Print_String(char *, enum E_Font);

  /**
   * @brief   Mp3 LCD Print to [x, p] position
   *
   * @param   uint8_t x {0 -> END_COLUMN_ADDR} defined in ssd1306.h
   * @param   uint8_t page {0 -> END_PAGE_ADDR} defined in ssd1306.h
   * @param   char * string
   * @param   enum E_Font / defined in ssd1306.h
   *
   * @return  void
   */
  void UI_Print_to_XY(uint8_t, uint8_t, char *, enum E_Font);

/**
   * @brief   Mp3 LCD Print Title
   *
   * @param   char *
   *
   * @return  void
   */
  void UI_Print_Title(char *);

  /**
   * @brief   Mp3 LCD Print FRAME
   *
   * @param   void
   *
   * @return  void
   */
  void UI_Print_Frame(void);

  /**
   * @brief   Mp3 LCD Print Menu
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void UI_Print_Menu(uint8_t);

  /**
   * @brief   Mp3 LCD Print all songs with highligted current song
   *
   * @param   FAT32_t * FAT32
   * @param   uint8_t current position
   * @param   UI_Files_t *
   *
   * @return  uint8_t
   */
  void UI_Print_Songs(FAT32_t *, uint8_t, UI_Files_t *);

  /**
   * @brief   Mp3 LCD Print File Name
   *
   * @param   FAT32_t * FAT32
   * @param   uint8_t current position
   * @param   UI_Files_t * UI_files struct
   *
   * @return  void
   */
  void UI_Print_File_Name(char *, char *, enum E_Font);

  /**
   * @brief   Mp3 LCD Print Volume
   *
   * @param   uint8_t current volume
   * @param   uint8_t max value
   * @param   char * title
   *
   * @return  void
   */
  void UI_Print_Vol_or_Tone(uint8_t, uint8_t, char *);

  /**
   * @brief   Mp3 ERROR
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void UI_Print_Error(uint8_t);

  /**
   * @brief   Mp3 LCD Draw Line
   *
   * @param   uint8_t x {0 -> END_COLUMN_ADDRESS}
   * @param   uint8_t y {0 -> END_PAGE_ADDR}
   * @param   uint8_t width {(x + width) <= END_COLUMN_ADDRESS}
   * @param   enum E_line {TOP, TOPDOUBLE, ... BOTTOM, BOTTOMDOUBLE} see enum E_Line 
   *
   * @return  void
   */
  void UI_Draw_Line_Horizontal(uint8_t, uint8_t, uint8_t, enum E_Line);

  /**
   * @brief   Mp3 LCD Print Loading Bar
   *
   * @param   uint8_t x
   * @param   uint8_t y
   * @param   uint8_t width
   * @param   uint8_t max
   *
   * @return  void
   */
  void UI_Draw_Loading_Bar(uint8_t x, uint8_t y, uint8_t width, uint8_t max);


  /**
   * @brief   Mp3 LCD Get Number Of MP3 Files
   *
   * @param   FAT32_t * FAT32
   *
   * @return  uint8_t
   */
  uint8_t UI_Get_Mp3Files(FAT32_t * FAT32);

#endif
