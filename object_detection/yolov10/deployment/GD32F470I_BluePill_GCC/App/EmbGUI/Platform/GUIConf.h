/*********************************************************************************************************
* ģ�����ƣ�GUIConf.h
* ժ    Ҫ��GUI����
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* ������ڣ�2021��07��01�� 
* ��    �ݣ�
* ע    �⣺���ļ���Ҫ������LCD.h��ߣ���Ȼ����ɺ궨���ظ��������
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/
#ifndef _GUI_CONF_H_
#define _GUI_CONF_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
//��������������
#define GUI_TOUCH_NUM_MAX     (5) //֧��5�㴥��

//����ʱ��Ļ�ߴ磨����һ��Ҫ���ڿ�ȣ�
#define GUI_X_PIXEL_SIZE      (800)
#define GUI_Y_PIXEL_SIZE      (480)

//��С�����С����(�߶�)
#define GUI_SMALLEST_FONT     (12)

//���ص��ʽ����
#define GUI_COLOR_TYPE_RGB555 (0) //RGB555
#define GUI_COLOR_TYPE_RGB565 (1) //RGB565
#define GUI_COLOR_TYPE_RGB888 (2) //RGB888
#define GUI_COLOR_TYPE_BGR888 (3) //BGR888
#define GUI_COLOR_TYPE GUI_COLOR_TYPE_RGB565

//��ɫ����
#ifndef GUI_COLOR_WHITE
  #if (GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_WHITE (0xFFFF)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_WHITE (0xFFFFFF)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_WHITE (0xFFFFFF)
  #endif
#endif

//��ɫ����
#ifndef GUI_COLOR_BLACK
  #if (GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_BLACK (0x0000)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_BLACK (0x000000)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_BLACK (0x000000)
  #endif
#endif

//��ɫ����
#ifndef GUI_COLOR_BLUE
  #if(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_BLUE (0x001F)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_BLUE (0x0000FF)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_BLUE (0xFF0000)
  #endif
#endif

//��ɫ����
#ifndef GUI_COLOR_GREEN
  #if(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_GREEN (0x07E0)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_GREEN (0x00FF00)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_GREEN (0x00FF00)
  #endif
#endif

//��ɫ����
#ifndef GUI_COLOR_ORANGE
  #if(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_ORANGE (0xFDCA)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_ORANGE (0xFFA500)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_ORANGE (0x00A5FF)
  #endif
#endif

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/
//���嶨��
typedef enum
{
  GUI_FONT_ASCII_12, //6x12��ASCII��
  GUI_FONT_ASCII_16, //8x16��ASCII��
  GUI_FONT_ASCII_24, //12x24��ASCII��
}EnumGUIFont;

//ͼƬ��ʽ����
typedef enum
{
  GUI_IMAGE_TYPE_BMP,   //λͼ
  GUI_IMAGE_TYPE_JPEG,  //JPEGͼƬ
  GUI_IMAGE_TYPE_PNG,   //PNGͼƬ
  GUI_IMAGE_TYPE_GIF,   //GIFͼƬ
  GUI_IMAGE_TYPE_CLEAR, //����ʾͼƬ���ؼ���ȫ͸��
  GUI_IMAGE_TYPE_NULL,  //��Чֵ
}EnumGUIImageType;

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/


#endif
