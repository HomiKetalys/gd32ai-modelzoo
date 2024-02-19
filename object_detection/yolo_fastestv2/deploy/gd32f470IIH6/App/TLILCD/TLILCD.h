/*********************************************************************************************************
* 模块名称：TLILCD.h
* 摘    要：LCD驱动模块
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日 
* 内    容：
* 注    意：默认使用RGB565格式，扫描时默认横屏扫描
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/
#ifndef _TLI_LCD_H_
#define _TLI_LCD_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//颜色定义
#define LCD_COLOR_WHITE          (0xFFFF)
#define LCD_COLOR_BLACK          (0x0000)
#define LCD_COLOR_GREY           (0xF7DE)
#define LCD_COLOR_BLUE           (0x001F)
#define LCD_COLOR_BLUE2          (0x051F)
#define LCD_COLOR_RED            (0xF800)
#define LCD_COLOR_MAGENTA        (0xF81F)
#define LCD_COLOR_GREEN          (0x07E0)
#define LCD_COLOR_CYAN           (0x7FFF)
#define LCD_COLOR_YELLOW         (0xFFE0)

//屏幕宽度和高度定义
#define LCD_PIXEL_WIDTH          ((u32)800)
#define LCD_PIXEL_HEIGHT         ((u32)480)

//LCD 参数，行时序按像素点计算，帧时序按照行数计算
#define HORIZONTAL_SYNCHRONOUS_PULSE  48  //行同步
#define HORIZONTAL_BACK_PORCH         88  //显示后沿
#define ACTIVE_WIDTH                  800 //显示区域
#define HORIZONTAL_FRONT_PORCH        40  //显示前沿

#define VERTICAL_SYNCHRONOUS_PULSE    3   //场同步
#define VERTICAL_BACK_PORCH           32  //显示后沿
#define ACTIVE_HEIGHT                 480 //显示区域
#define VERTICAL_FRONT_PORCH          13  //显示前沿

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/
//层枚举
typedef enum
{
  LCD_LAYER_BACKGROUND = 0, //背景层
  LCD_LAYER_FOREGROUND,     //前景层
  LCD_LAYER_MAX,            //层总数
}EnumTLILCDLayer;

//LCD方向枚举
typedef enum
{
  LCD_SCREEN_HORIZONTAL = 0, //横屏，默认
  LCD_SCREEN_VERTICAL,       //竖屏
}EnumTLILCDDir;

//LCD设备结构体，按4直接对齐方式
typedef struct
{
  u32             pWidth;                //LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入
  u32             pHeight;               //LCD面板的高度,固定参数,不随显示方向改变
  EnumTLILCDDir   dir;                   //层显示方向
  u32             width[LCD_LAYER_MAX];  //层窗口宽度
  u32             height[LCD_LAYER_MAX]; //层窗口高度
  EnumTLILCDLayer currentLayer;          //当前层
  u16*            frameBuf;              //当前显存首地址（分有前景和背景两个显存）
  u32             backFrameAddr;         //背景显存地址
  u32             foreFrameAddr;         //前景显存地址
  u32             pixelSize;             //一个像素点字节数
}StructTLILCDDev __attribute__ ((aligned (4)));

//字体枚举
typedef enum
{
  LCD_FONT_12, //6x12 ASCII码或12x12中文
  LCD_FONT_16, //8x16 ASCII码或16x16中文
  LCD_FONT_24, //12x24 ASCII码或24x24中文
}EnumTLILCDFont;

//显示方式枚举
typedef enum
{
  LCD_TEXT_NORMAL, //通用字体（含背景色）
  LCD_TEXT_TRANS,  //显示透明字体（无背景色）
}EnumTLILCDTextMode;

extern StructTLILCDDev g_structTLILCDDev;



/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
//硬件相关
void InitLCD(void);
void LCDLayerWindowSet(EnumTLILCDLayer layer, u32 x, u32 y, u32 width, u32 height);
void LCDLayerEnable(EnumTLILCDLayer layer);
void LCDLayerDisable(EnumTLILCDLayer layer);
void LCDLayerSwitch(EnumTLILCDLayer layer);
void LCDTransparencySet(EnumTLILCDLayer layer, u8 trans);
void LCDDisplayOn(void);
void LCDDisplayOff(void);
void LCDWaitHIdle(void);
void LCDWaitVIdle(void);

//LCD常用API
void LCDDisplayDir(EnumTLILCDDir dir);
void LCDClear(u32 color);
void LCDDrawPoint(u32 x, u32 y, u32 color);
u32  LCDReadPoint(u32 x, u32 y);
void LCDFill(u32 x, u32 y, u32 width, u32 height, u32 color);
void LCDFillPixel(u32 x0, u32 y0, u32 x1, u32 y1, u32 color);
void LCDColorFill(u32 x, u32 y, u32 width, u32 height, u16* color);
void LCDColorFillPixel(u32 x0, u32 y0, u32 x1, u32 y1, u16* color);
void LCDDrawLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 color);
void LCDDrawRectangle(u32 x1, u32 y1, u32 x2, u32 y2, u32 color);
void LCDDrawCircle(u32 x0, u32 y0, u32 r, u32 color);

//字符串显示相关
void LCDShowChar(u32 x, u32 y, EnumTLILCDFont font, EnumTLILCDTextMode mode, u32 textColor, u32 backColor, u32 code);
void LCDShowString(u32 x, u32 y, u32 width, u32 height, EnumTLILCDFont font, EnumTLILCDTextMode mode, u32 textColor, u32 backColor, char* string);

//窗口保存与绘制
void LCDWindowSave(u32 x, u32 y, u32 width, u32 height, void* saveBuf);
void LCDWindowFill(u32 x, u32 y, u32 width, u32 height, void* imageBuf);

//RGB888与RGB565转换
void RGB565ToRGB888A(u16 rgb565, u8* r, u8* g, u8* b);
u32  RGB565ToRGB888B(u16 rgb565);
u32  RGB888ToRGB565A(u8 r, u8 g, u8 b);
u32  RGB888ToRGB565B(u32 rgb888);

#endif
