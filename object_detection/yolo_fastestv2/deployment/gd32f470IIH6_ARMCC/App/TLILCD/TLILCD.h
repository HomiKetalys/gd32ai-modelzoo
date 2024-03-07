/*********************************************************************************************************
* ģ�����ƣ�TLILCD.h
* ժ    Ҫ��LCD����ģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* ������ڣ�2020��01��01�� 
* ��    �ݣ�
* ע    �⣺Ĭ��ʹ��RGB565��ʽ��ɨ��ʱĬ�Ϻ���ɨ��
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/
#ifndef _TLI_LCD_H_
#define _TLI_LCD_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
//��ɫ����
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

//��Ļ��Ⱥ͸߶ȶ���
#define LCD_PIXEL_WIDTH          ((u32)800)
#define LCD_PIXEL_HEIGHT         ((u32)480)

//LCD ��������ʱ�����ص���㣬֡ʱ������������
#define HORIZONTAL_SYNCHRONOUS_PULSE  48  //��ͬ��
#define HORIZONTAL_BACK_PORCH         88  //��ʾ����
#define ACTIVE_WIDTH                  800 //��ʾ����
#define HORIZONTAL_FRONT_PORCH        40  //��ʾǰ��

#define VERTICAL_SYNCHRONOUS_PULSE    3   //��ͬ��
#define VERTICAL_BACK_PORCH           32  //��ʾ����
#define ACTIVE_HEIGHT                 480 //��ʾ����
#define VERTICAL_FRONT_PORCH          13  //��ʾǰ��

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/
//��ö��
typedef enum
{
  LCD_LAYER_BACKGROUND = 0, //������
  LCD_LAYER_FOREGROUND,     //ǰ����
  LCD_LAYER_MAX,            //������
}EnumTLILCDLayer;

//LCD����ö��
typedef enum
{
  LCD_SCREEN_HORIZONTAL = 0, //������Ĭ��
  LCD_SCREEN_VERTICAL,       //����
}EnumTLILCDDir;

//LCD�豸�ṹ�壬��4ֱ�Ӷ��뷽ʽ
typedef struct
{
  u32             pWidth;                //LCD���Ŀ��,�̶�����,������ʾ����ı�,���Ϊ0,˵��û���κ�RGB������
  u32             pHeight;               //LCD���ĸ߶�,�̶�����,������ʾ����ı�
  EnumTLILCDDir   dir;                   //����ʾ����
  u32             width[LCD_LAYER_MAX];  //�㴰�ڿ��
  u32             height[LCD_LAYER_MAX]; //�㴰�ڸ߶�
  EnumTLILCDLayer currentLayer;          //��ǰ��
  u16*            frameBuf;              //��ǰ�Դ��׵�ַ������ǰ���ͱ��������Դ棩
  u32             backFrameAddr;         //�����Դ��ַ
  u32             foreFrameAddr;         //ǰ���Դ��ַ
  u32             pixelSize;             //һ�����ص��ֽ���
}StructTLILCDDev __attribute__ ((aligned (4)));

//����ö��
typedef enum
{
  LCD_FONT_12, //6x12 ASCII���12x12����
  LCD_FONT_16, //8x16 ASCII���16x16����
  LCD_FONT_24, //12x24 ASCII���24x24����
}EnumTLILCDFont;

//��ʾ��ʽö��
typedef enum
{
  LCD_TEXT_NORMAL, //ͨ�����壨������ɫ��
  LCD_TEXT_TRANS,  //��ʾ͸�����壨�ޱ���ɫ��
}EnumTLILCDTextMode;

extern StructTLILCDDev g_structTLILCDDev;



/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
//Ӳ�����
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

//LCD����API
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

//�ַ�����ʾ���
void LCDShowChar(u32 x, u32 y, EnumTLILCDFont font, EnumTLILCDTextMode mode, u32 textColor, u32 backColor, u32 code);
void LCDShowString(u32 x, u32 y, u32 width, u32 height, EnumTLILCDFont font, EnumTLILCDTextMode mode, u32 textColor, u32 backColor, char* string);

//���ڱ��������
void LCDWindowSave(u32 x, u32 y, u32 width, u32 height, void* saveBuf);
void LCDWindowFill(u32 x, u32 y, u32 width, u32 height, void* imageBuf);

//RGB888��RGB565ת��
void RGB565ToRGB888A(u16 rgb565, u8* r, u8* g, u8* b);
u32  RGB565ToRGB888B(u16 rgb565);
u32  RGB888ToRGB565A(u8 r, u8 g, u8 b);
u32  RGB888ToRGB565B(u32 rgb888);

#endif
