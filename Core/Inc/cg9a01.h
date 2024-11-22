#include "main.h"
#define LCD_W 240
#define LCD_H 240

#define GC9A01_TFTWIDTH 240
#define GC9A01_TFTHEIGHT 240

#define GC9A01_RST_DELAY 120    ///< delay ms wait for reset finish
#define GC9A01_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define GC9A01_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define GC9A01_NOP 0x00
#define GC9A01_SWRESET 0x01
#define GC9A01_RDDID 0x04
#define GC9A01_RDDST 0x09

#define GC9A01_SLPIN 0x10
#define GC9A01_SLPOUT 0x11
#define GC9A01_PTLON 0x12
#define GC9A01_NORON 0x13

#define GC9A01_INVOFF 0x20
#define GC9A01_INVON 0x21
#define GC9A01_DISPOFF 0x28
#define GC9A01_DISPON 0x29

#define GC9A01_CASET 0x2A
#define GC9A01_RASET 0x2B
#define GC9A01_RAMWR 0x2C
#define GC9A01_RAMRD 0x2E

#define GC9A01_PTLAR 0x30
#define GC9A01_COLMOD 0x3A
#define GC9A01_MADCTL 0x36

#define GC9A01_MADCTL_MY 0x80
#define GC9A01_MADCTL_MX 0x40
#define GC9A01_MADCTL_MV 0x20
#define GC9A01_MADCTL_ML 0x10
#define GC9A01_MADCTL_RGB 0x00

#define GC9A01_RDID1 0xDA
#define GC9A01_RDID2 0xDB
#define GC9A01_RDID3 0xDC
#define GC9A01_RDID4 0xDD

#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0xF81F
#define GRED             0xFFE0
#define GBLUE            0x07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0xBC40
#define BRRED            0xFC07
#define GRAY             0x8430
#define DARKBLUE         0x01CF
#define LIGHTBLUE        0x7D7C
#define GRAYBLUE         0x5458
#define LIGHTGREEN       0x841F
#define LGRAY            0xC618
#define LGRAYBLUE        0xA651
#define LBBLUE           0x2B12

// Colors of the rainbow
#define VIOLET           0x8010
#define INDIGO           0x4810
#define RAINBOW_BLUE     0x001F
#define RAINBOW_GREEN    0x07E0
#define RAINBOW_YELLOW   0xFFE0
#define RAINBOW_ORANGE   0xFD20
#define RAINBOW_RED      0xF800

// Additional colors
#define ORANGE           0xFD20
#define PINK             0xF97F
#define GOLD             0xFEA0
#define SILVER           0xC618
#define PURPLE           0x780F
#define TURQUOISE        0x471A
#define MINT             0xB7E0
#define PEACH            0xF9E1
#define BEIGE            0xF7BB
#define OLIVE            0x8400
#define CORAL            0xFBEA
#define AQUA             0x07FF

#define USE_HORIZONTAL 1  //Set the display direction 0,1,2,3	four directions

void SPI_write(uint8_t data);
void GC9A01_Write_Cmd_Data (uint8_t CMDP);
void GC9A01_Write_Cmd(uint8_t CMD);
void GC9A01_Write_Data_U16(uint16_t y);
void GC9A01_Write_Data(uint8_t DH,uint8_t DL);
void GC9A01_Write_Bytes(uint8_t * pbuff, uint16_t size);
void GC9A01_Initial(void);
void GC9A01_ClearScreen(uint16_t bColor);
void GC9A01_ClearWindow(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, uint16_t bColor);
void GC9A01_SetPos(uint8_t Xstart, uint8_t Ystart, uint8_t Xend, uint8_t Yend);
void GC9A01_show_picture(uint16_t *picture, uint16_t x,uint16_t y,uint16_t b,uint16_t h, uint8_t widht, uint8_t height);
void GC9A01_DrawPixel_2x2(uint8_t x, uint8_t y, uint16_t color);
void GC9A01_draw_line(uint16_t color, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2);
void GC9A01_DrawRect(uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void GC9A01_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void GC9A01_FilledDrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void GC9A01_SetTextColor(uint16_t color);
void GC9A01_SetBackColor(uint16_t color);
void GC9A01_SetFont(sFONT *pFonts);
void GC9A01_DrawChar(uint16_t x, uint16_t y, uint8_t c);
void GC9A01_String(uint16_t x,uint16_t y, char *str);
void GC9A01_Rainbow_String(uint16_t x,uint16_t y, char *str);
//void ShowMenu(struct MenuMember Members[7]);



