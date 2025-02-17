/**
 * --------------------------------------------------------------------------------------+
 * @brief       MP3-SD-FAT32 PLAYER Interface
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2024 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        14.03.2024
 * @file        mp3.c
 * @version     1.0.0
 * @test        AVR Atmega328p
 *
 * @depend      sd.h, fat32.h. vs1053.h, ssd1306.h
 * --------------------------------------------------------------------------------------+
 * @interface   
 * @pins
 *
 * @sources
 */
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "../fat32/fat32.h"
#include "../lcd/ssd1306.h"

// global variables
extern unsigned short int _indexPage;

/**
 * @brief   Mp3 LCD Set Position
 *
 * @param   uint8_t x
 * @param   uint8_t y
 *
 * @return  void
 */
static inline void UI_Set_Position(uint8_t x, uint8_t y)
{
  SSD1306_SetPosition(x, y);
}

/**
 * @brief   Mp3 LCD Print Character
 *
 * @param   char character
 * @param   enum E_Font / defined in ssd1306.h
 *
 * @return  void
 */
static inline void UI_Print_Char(char character, enum E_Font font)
{
  SSD1306_DrawChar(character, font);
}

/**
 * @brief   Mp3 Init
 *
 * @param   FAT32_t * FAT
 *
 * @return  uint8_t
 */
uint8_t UI_Init(FAT32_t *FAT32)
{
  // Init LCD SSD1306
  // ----------------------------------------------------------------
  if (SSD1306_SUCCESS != SSD1306_Init(SSD1306_ADDR)) {
    return SSD1306_ERROR;
  }
  // Init SD
  // ----------------------------------------------------------------
  if (FAT32_ERROR == FAT32_Init(FAT32)) {
    return FAT32_ERROR;
  }

  return UI_SUCCESS;
}

/**
 * @brief   Mp3 LCD Play & Show Song
 *
 * @param   FAT32_t * FAT32
 * @param   uint8_t song
 * @param   UI_Files_t * UI_files struct
 *
 * @return  void
 */
void UI_Show_Song(FAT32_t *FAT32, uint8_t songid, UI_Files_t *UI_Files)
{
  DE_t * File = FAT32_Get_File_Info(FAT32, (uint32_t) songid);

  uint8_t x = 36;
  uint8_t y = 6;

  uint8_t to = 8;

  while (to--){
    if (' ' != File->Name[to]){
      break;
    }
  }
  // Print title
  // ----------------------------------------------------------------  
  UI_Set_Position((UI_FRAME_MARGIN >> 1), 1);
  UI_Print_String(" --- NOW PLAYING ---", NORMAL);
  // Print time
  // ----------------------------------------------------------------
  UI_Clear_Pages(3, 6, UI_FRAME_MARGIN);
  UI_Set_Position(((128-(to+1)*6) >> 1)-12, 4);
  UI_Print_File_Name((char *) File->Name, (char *) File->Extension, NORMAL);
  UI_Print_to_XY(x, y, "00:00", BOLD);
}

/**
 * @brief   Mp3 LCD Clear Screen
 *
 * @param   void
 *
 * @return  void
 */
void UI_Clear_Screen(void)
{
  SSD1306_ClearScreen();
}

/**
 * @brief   Mp3 LCD Clear Pages From definded position to end
 *
 * @param   uint8_t from page
 * @param   uint8_t to page
 * @param   uint8_t margin
 *
 * @return  void
 */
void UI_Clear_Pages(uint8_t from, uint8_t to,uint8_t margin)
{
  SSD1306_ClearPages(from, to, margin);
}

/**
 * @brief   Mp3 LCD Print
 *
 * @param   char *
 * @param   enum E_Font / defined in ssd1306.h
 *
 * @return  void
 */
void UI_Print_String(char *str, enum E_Font font)
{
  SSD1306_DrawString(str, font);
}

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
void UI_Print_to_XY(uint8_t x, uint8_t p, char *str, enum E_Font font)
{
  UI_Set_Position(x, p);
  UI_Print_String(str, font);
}

/**
 * @brief   Mp3 LCD Print FRAME
 *
 * @param   void
 *
 * @return  void
 */
void UI_Print_Frame(void)
{
  SSD1306_DrawLineHorizontal(0, 0, END_COLUMN_ADDR, 0x03);
  SSD1306_DrawLineHorizontal(0, 2, END_COLUMN_ADDR, 0x18);
  SSD1306_DrawLineHorizontal(0, END_PAGE_ADDR, END_COLUMN_ADDR, 0x80);
  SSD1306_DrawLineVertical(0, 0, END_PAGE_ADDR);
  SSD1306_DrawLineVertical(END_COLUMN_ADDR, 0, END_PAGE_ADDR);
}

/**
 * @brief   Mp3 LCD Print Title
 *
 * @param   char * title
 *
 * @return  void
 */
void UI_Print_Title(char *title)
{
  UI_Print_to_XY(UI_FRAME_MARGIN, 1, title, NORMAL);
}

/**
 * @brief   Mp3 LCD Print Songs
 *
 * @param   FAT32_t * FAT32
 * @param   uint8_t current position
 * @param   UI_Files_t * UI_files struct
 *
 * @return  void
 */
void UI_Print_Songs(FAT32_t *FAT32, uint8_t current, UI_Files_t *UI_files)
{
  DE_t * file;
  char str[4];

  uint8_t row = 3;
  uint8_t page = (current - 1) / UI_files->Group;
  uint8_t start = page * UI_files->Group + 1;
  uint8_t end = (page < UI_files->Pages) ? (start + UI_files->Group) : (UI_files->Count + 1);

  if (UI_files->Page != page) {
    UI_Clear_Pages(row, 6, UI_FRAME_MARGIN);
    UI_files->Page = page;
  }

  UI_Clear_Pages(1, 1, UI_FRAME_MARGIN);
  UI_Set_Position(UI_FRAME_MARGIN, 1);
  UI_Print_String("FOLDER/[", NORMAL);
  itoa(UI_files->Page+1, str, 10);
  UI_Print_String(str, NORMAL);
  UI_Print_Char('-', NORMAL);
  itoa(UI_files->Pages, str, 10);
  UI_Print_String(str, NORMAL);
  UI_Print_Char(']', NORMAL);

  for (uint8_t i = start; i < end; i++) {
    UI_Set_Position(UI_FRAME_MARGIN, row++);    
    if (i == current) {
      UI_Print_Char('>', NORMAL);
    } else {
      UI_Print_Char(' ', NORMAL);
    }
    file = FAT32_Get_File_Info(FAT32, i);
    UI_Print_File_Name((char *) file->Name, (char *) file->Extension, NORMAL);
  }
}

/**
 * @brief   Mp3 LCD Print File Name
 *
 * @param   char *
 * @param   char *
 * @param   enum E_Font
 *
 * @return  void
 */
void UI_Print_File_Name(char *name, char *extension, enum E_Font font)
{
  uint8_t i = 8;
  uint8_t to = 0;

  while (i--){
    if (' ' != name[i]){
      to = i;
      break;
    }
  }

  for (i=0; i<=to; i++) { 
    UI_Print_Char(name[i], font); 
  }
  UI_Print_Char('.', font);
  UI_Print_String(extension, NORMAL);
}

/**
 * @brief   Mp3 ERROR
 *
 * @param   uint8_t
 *
 * @return  void
 */
void UI_Print_Error(uint8_t errorcode)
{
  switch (errorcode) {
    case SSD1306_ERROR:
      UI_Print_String("ERROR CODE: LCD", NORMAL);
      break;
    case FAT32_ERROR:
      UI_Print_String("ERROR CODE: FAT", NORMAL);
      break;
    default:
      UI_Print_String("ERROR CODE: NAN", NORMAL);
      break;
  }
}

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
void UI_Draw_Line_Horizontal(uint8_t x, uint8_t y, uint8_t width, enum E_Line line)
{
  SSD1306_DrawLineHorizontal(x, y, width, line);
}