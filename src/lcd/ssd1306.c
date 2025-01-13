/**
 * --------------------------------------------------------------------------------------+
 * @name        SSD1306 OLED Driver Library
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.10.2020
 * @file        ssd1306.c
 * @version     3.0
 * @test        AVR Atmega328p
 *
 * @depend      font.h, twi.h
 * --------------------------------------------------------------------------------------+
 * @version     Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0 -> simplified alphanumeric version
 * --------------------------------------------------------------------------------------+
 * @brief       Basic Setup for OLED Display
 */

// @includes
#include "ssd1306.h"

// @const uint8_t - List of init commands according to datasheet SSD1306
const uint8_t INIT_SSD1306[] PROGMEM = {
  17,                                                             // number of initializers
  //SSD1306_RESET, 0,                                               // 0xE4 = Software Reset?
  SSD1306_DISPLAY_OFF, 0,                                         // 0xAE = Set Display OFF
  SSD1306_SET_MUX_RATIO, 1, 0x3F,                                 // 0xA8 - 0x3F for 128 x 64 version (64MUX)
                                                                  //      - 0x1F for 128 x 32 version (32MUX)
  SSD1306_MEMORY_ADDR_MODE, 1, 0x00,                              // 0x20 = Set Memory Addressing Mode
                                                                  // 0x00 - Horizontal Addressing Mode
                                                                  // 0x01 - Vertical Addressing Mode
                                                                  // 0x02 - Page Addressing Mode (RESET)
  SSD1306_SET_START_LINE, 0,                                      // 0x40
  SSD1306_DISPLAY_OFFSET, 1, 0x00,                                // 0xD3
  SSD1306_SEG_REMAP_OP, 0,                                        // 0xA0 / remap 0xA1
  SSD1306_COM_SCAN_DIR_OP, 0,                                     // 0xC0 / remap 0xC8
  SSD1306_COM_PIN_CONF, 1, 0x12,                                  // 0xDA, 0x12 - Disable COM Left/Right remap, Alternative COM pin configuration
                                                                  //       0x12 - for 128 x 64 version
                                                                  //       0x02 - for 128 x 32 version
  SSD1306_SET_CONTRAST, 1, 0x7F,                                  // 0x81, 0x7F - reset value (max 0xFF)
  SSD1306_DIS_ENT_DISP_ON, 0,                                     // 0xA4
  SSD1306_DIS_NORMAL, 0,                                          // 0xA6
  SSD1306_SET_OSC_FREQ, 1, 0x80,                                  // 0xD5, 0x80 => D=1; DCLK = Fosc / D <=> DCLK = Fosc
  SSD1306_SET_PRECHARGE, 1, 0xc2,                                 // 0xD9, higher value less blinking
                                                                  // 0xC2, 1st phase = 2 DCLK,  2nd phase = 13 DCLK
  SSD1306_VCOM_DESELECT, 1, 0x20,                                 // Set V COMH Deselect, reset value 0x22 = 0,77xUcc
  SSD1306_SET_CHAR_REG, 1, 0x14,                                  // 0x8D, Enable charge pump during display on
  SSD1306_DEACT_SCROLL, 0,                                        // 0x2E
  SSD1306_DISPLAY_ON, 0                                           // 0xAF = Set Display ON
};

unsigned short int _indexCol = START_COLUMN_ADDR;                 // @var global - cache index column
unsigned short int _indexPage = START_PAGE_ADDR;                  // @var global - cache index page

/**
 * @desc    SSD1306 Send Start and SLAW request
 *
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SSD1306_Send_StartAndSLAW (uint8_t address)
{
  uint8_t status = INIT_STATUS;

  // TWI START
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Start ();
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  // TWI SLAW
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_SLAW (address);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Send command
 *
 * @param   uint8_t command
 *
 * @return  uint8_t
 */
uint8_t SSD1306_Send_Command(uint8_t command)
{
  uint8_t status = INIT_STATUS;

  // TWI send control byte
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data(SSD1306_COMMAND);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  // TWI send command
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data(command);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Init
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_Init (uint8_t address)
{
  const uint8_t * list = INIT_SSD1306;
  uint8_t status = INIT_STATUS;
  uint8_t arguments;
  uint8_t commands = pgm_read_byte (list++);

  // TWI: Init
  // -------------------------------------------------------------------------------------
  TWI_Init ();

  // TWI: start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  if (SSD1306_SUCCESS != status) {
    return status;
  }

  // SW RESET
  // -----------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_RESET);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  _delay_ms (1);

  // Commands & Arguments
  // -------------------------------------------------------------------------------------
  while (commands--) {
    // Command
    // -----------------------------------------------------------------------------------
    status = SSD1306_Send_Command(pgm_read_byte(list++));
    if (SSD1306_SUCCESS != status) {
      return status;
    }
    // Arguments
    // -----------------------------------------------------------------------------------
    arguments = pgm_read_byte (list++);
    while (arguments--) {
      status = SSD1306_Send_Command(pgm_read_byte(list++));
      if (SSD1306_SUCCESS != status) {
        return status;
      }
    }
  }
  // TWI: Stop
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Normal colors
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_NormalScreen (void)
{
  uint8_t status = INIT_STATUS;

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  // TWI send command
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_DIS_NORMAL);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Inverse colors
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_InverseScreen (void)
{
  uint8_t status = INIT_STATUS;

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  // TWI send command
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_DIS_INVERSE);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Clear screen
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_ClearScreen(void)
{
  return SSD1306_ClearPages(0, END_PAGE_ADDR, 0);
}

/**
 * @desc    SSD1306 Clear screen
 *
 * @param   uint8_t from page
 * @param   uint8_t to page
 * @param   uint8_t x margin
 *
 * @return  uint8_t
 */
uint8_t SSD1306_ClearPages(uint8_t from, uint8_t to, uint8_t margin)
{
  uint8_t status = INIT_STATUS;

  if (from > to) {
    return SSD1306_ERROR;
  }
  
  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  // COLUMN
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_SET_COLUMN_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(margin);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(END_COLUMN_ADDR-margin);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexCol = margin;
  // PAGE
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_SET_PAGE_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(from);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(to);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexPage = 0;
  // TWI control byte data stream
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  //  send clear byte to memory lcd
  // -------------------------------------------------------------------------------------
  for (uint16_t i=0; i<=((to-from+1) * (END_COLUMN_ADDR - 2*margin)); i++) {
    status = TWI_MT_Send_Data(CLEAR_COLOR);
    if (SSD1306_SUCCESS != status) {
      return status;
    }
  }
  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop();

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Set position
 *
 * @param   uint8_t column -> 0 ... 127
 * @param   uint8_t column -> 0 ... 127
 * @param   uint8_t page -> 0 ... 3 or 7
 * @param   uint8_t page -> 0 ... 3 or 7
 *
 * @return  void
 */
uint8_t SSD1306_SetWindow(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
{
  uint8_t status = INIT_STATUS;

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  // COLUMN
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_SET_COLUMN_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(x1);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(x2);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexCol = x1;
  // PAGE
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_SET_PAGE_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(y1);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(y2);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexPage = y1;

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop();

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Set position
 *
 * @param   uint8_t column -> 0 ... 127
 * @param   uint8_t page -> 0 ... 7
 *
 * @return  void
 */
uint8_t SSD1306_SetPosition(uint8_t x, uint8_t y)
{
  uint8_t status = INIT_STATUS;
 
  status = SSD1306_SetWindow (x, END_COLUMN_ADDR, y, END_PAGE_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Update text poisition - this ensure that character will not be divided
 *          at the end of row, the whole character will be depicted on the new row
 *
 * @param   uint8_t column
 * @param   uint8_t page
 *
 * @return  uint8_t
 */
uint8_t SSD1306_UpdatePosition(uint8_t x, uint8_t p)
{
  uint8_t status = INIT_STATUS;

  // check position
  // -------------------------------------------------------------------------------------
  if (x > END_COLUMN_ADDR) {
    // last page not reached
    // -----------------------------------------------------------------------------------
    if (p < END_PAGE_ADDR) {
      _indexCol = 2;                                              // update column
      _indexPage = _indexPage + 1;                                // update page
      status = SSD1306_SetPosition (_indexCol, _indexPage);       // update position
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
    }
    // last page reached
    // -----------------------------------------------------------------------------------
    if (p >= END_PAGE_ADDR) {
      return SSD1306_ERROR;
    }
  }

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Draw character
 *
 * @param   char character
 * @param   enum font 
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawChar(char ch, enum E_Font font)
{
  uint8_t byte;
  uint8_t status = INIT_STATUS;
  uint8_t i = 0;
  uint8_t next_x;
  uint8_t next_p;
  uint8_t mask = 0x00;

  //  NORMAL FONT
  // -------------------------------------------------------------------------------------
  if ((font & 0x0f) == NORMAL) { 

    if (font & 0xf0) {                                            // underline?
      mask = 0x80;                                                // set underline mask
    }

    next_x = _indexCol + CHARS_COLS_LENGTH;                       // next column
    next_p = _indexPage;                                          // next page

    // UPDATE / CHECK TEXT POSITION
    // -----------------------------------------------------------------------------------
    status = SSD1306_UpdatePosition (next_x, next_p);
    if (SSD1306_SUCCESS != status) {
      return status;
    }

    // TWI START & SLAW
    // -----------------------------------------------------------------------------------
    status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);
    if (SSD1306_SUCCESS != status) {
      return status;
    }
    // TWI control byte data stream
    // -----------------------------------------------------------------------------------
    status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
    if (SSD1306_SUCCESS != status) {
      return status;
    }

    while (i < CHARS_COLS_LENGTH) {
      byte = pgm_read_byte (&FONTS[ch-32][i++]);
      status = TWI_MT_Send_Data(byte | mask);
/*
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
*/
      _indexCol++;                                                // update global col
    }
  //  BOLD FONT
  // -------------------------------------------------------------------------------------
  } else if ((font & 0x0f) == BOLD) {

    if (font & 0xf0) {                                            // underline?
      mask = 0x80;                                                // set underline mask
    }

    next_x = _indexCol + (CHARS_COLS_LENGTH << 1);
    next_p = _indexPage;

    // UPDATE / CHECK TEXT POSITION
    // -----------------------------------------------------------------------------------
    status = SSD1306_UpdatePosition(next_x, next_p);
    if (SSD1306_SUCCESS != status) {
      return status;
    }

    // TWI START & SLAW
    // -----------------------------------------------------------------------------------
    status = SSD1306_Send_StartAndSLAW(SSD1306_ADDR);
    if (SSD1306_SUCCESS != status) {
      return status;
    }
    // TWI control byte data stream
    // -----------------------------------------------------------------------------------
    status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
    if (SSD1306_SUCCESS != status) {
      return status;
    }

    while (i < CHARS_COLS_LENGTH) {
      byte = pgm_read_byte (&FONTS[ch-32][i++]);
      status = TWI_MT_Send_Data(byte | mask);
      if (SSD1306_SUCCESS != status) {
        return status;
      }
      status = TWI_MT_Send_Data(byte | mask);
      if (SSD1306_SUCCESS != status) {
        return status;
      }
      _indexCol = _indexCol + 2;
    }
  } 
  //  send empty column to memory lcd
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data(mask);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexCol++;

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop();

  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Draw String
 *
 * @param   char * string
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawString(char *str, enum E_Font font)
{
  uint8_t i = 0;

  // send characters of string
  // -------------------------------------------------------------------------------------
  while (str[i] != '\0') {
    SSD1306_DrawChar(str[i++], font);
  }

  return SSD1306_SUCCESS;
}

/**
 * @brief   SSD1306 Draw line vertical
 *
 * @param   uint8_t x {0 -> END_COLUMN_ADDRESS}
 * @param   uint8_t y {0 -> END_PAGE_ADDR}
 * @param   uint8_t height
 * 
 * @return  uint8_t
 */
uint8_t SSD1306_DrawLineVertical(uint8_t x, uint8_t y, uint8_t height)
{
  uint8_t i = 0;
  uint8_t status = INIT_STATUS;

  if ((y + height) > END_PAGE_ADDR) {
    return SSD1306_ERROR;
  }

  // TWI START & SLAW
  // -----------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }

  // COLUMN
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_SET_COLUMN_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(x);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(x);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexCol = x;
  // PAGE
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_SET_PAGE_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(y);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(y + height);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexPage = y;
  // TWI control byte data stream
  // -----------------------------------------------------------------------------------
  status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  while (i++ <= height) {
    status = TWI_MT_Send_Data(0xff);
    if (SSD1306_SUCCESS != status) {
      return status;
    }
  }

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop();

  return SSD1306_SUCCESS;
}

/**
 * @brief   SSD1306 Draw line horizontal
 *
 * @param   uint8_t x {0 -> END_COLUMN_ADDRESS}
 * @param   uint8_t y {0 -> END_PAGE_ADDR}
 * @param   uint8_t width {(x + width) <= END_COLUMN_ADDRESS}
 * @param   enum E_line {TOP, TOPDOUBLE, ... BOTTOM, BOTTOMDOUBLE} see enum E_Line 
 * 
 * @return  uint8_t
 */
uint8_t SSD1306_DrawLineHorizontal(uint8_t x, uint8_t y, uint8_t width, enum E_Line line)
{
  uint8_t status = INIT_STATUS;
  uint8_t i = 0;

  if ((x + width) > END_COLUMN_ADDR) {
    return SSD1306_ERROR;
  }

  // TWI START & SLAW
  // -----------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW(SSD1306_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }

  // COLUMN
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_SET_COLUMN_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(x);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(x + width);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexCol = x;
  // PAGE
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command(SSD1306_SET_PAGE_ADDR);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(y);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  status = SSD1306_Send_Command(y);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }
  _indexPage = y;
  // TWI control byte data stream
  // -----------------------------------------------------------------------------------
  status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
  if (SSD1306_SUCCESS != status) {  
    return status;  
  }

  while (i++ < width) {
    status = TWI_MT_Send_Data(line);
    if (SSD1306_SUCCESS != status) {
      return status;
    }
    _indexCol++;
  }

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop();

  return SSD1306_SUCCESS;
}

/**
 * @brief   SSD1306 Horizontal Scroll
 *
 * @param   uint8_t address
 * @param   uint8_t start page
 * @param   uint8_t end page
 *
 * @return  uint8_t
 */
uint8_t SSD1306_HorizontalScroll(uint8_t address, uint8_t start, uint8_t end)
{
  uint8_t status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW(address);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // send command
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(SSD1306_SCROLL_HOR_LEFT);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // dummy BYTE
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(0x00);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // Start PAGE
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(start);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // Speed
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(0x00);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // End PAGE
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(end);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // dummy BYTE
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(0x00);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // dummy BYTE
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(0xFF);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // Start Scroll
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(SSD1306_ACTIVE_SCROLL);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // TWI: Stop
  // -------------------------------------------------------------------------------------
  TWI_Stop();

  return SSD1306_SUCCESS;
}

/**
 * @brief   SSD1306 Horizontal Scroll Stop
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_HorizontalScrollStop(uint8_t address)
{
  uint8_t status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW(address);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // Stop Scroll
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command(SSD1306_DEACT_SCROLL);
  if (SSD1306_SUCCESS != status) {
    return status;
  }
  // TWI: Stop
  // -------------------------------------------------------------------------------------
  TWI_Stop();

  return SSD1306_SUCCESS;
}