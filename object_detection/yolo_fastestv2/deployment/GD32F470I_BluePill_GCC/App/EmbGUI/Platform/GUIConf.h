/*********************************************************************************************************
* 模块名称：GUIConf.h
* 摘    要：GUI配置
* 当前版本：1.0.0
* 作    者：Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* 完成日期：2021年07月01日 
* 内    容：
* 注    意：该文件需要放置在LCD.h后边，不然会造成宏定义重复定义错误
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/
#ifndef _GUI_CONF_H_
#define _GUI_CONF_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//触摸点数量定义
#define GUI_TOUCH_NUM_MAX     (5) //支持5点触碰

//横屏时屏幕尺寸（长度一定要大于宽度）
#define GUI_X_PIXEL_SIZE      (800)
#define GUI_Y_PIXEL_SIZE      (480)

//最小字体大小定义(高度)
#define GUI_SMALLEST_FONT     (12)

//像素点格式定义
#define GUI_COLOR_TYPE_RGB555 (0) //RGB555
#define GUI_COLOR_TYPE_RGB565 (1) //RGB565
#define GUI_COLOR_TYPE_RGB888 (2) //RGB888
#define GUI_COLOR_TYPE_BGR888 (3) //BGR888
#define GUI_COLOR_TYPE GUI_COLOR_TYPE_RGB565

//白色定义
#ifndef GUI_COLOR_WHITE
  #if (GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_WHITE (0xFFFF)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_WHITE (0xFFFFFF)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_WHITE (0xFFFFFF)
  #endif
#endif

//黑色定义
#ifndef GUI_COLOR_BLACK
  #if (GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_BLACK (0x0000)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_BLACK (0x000000)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_BLACK (0x000000)
  #endif
#endif

//蓝色定义
#ifndef GUI_COLOR_BLUE
  #if(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_BLUE (0x001F)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_BLUE (0x0000FF)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_BLUE (0xFF0000)
  #endif
#endif

//绿色定义
#ifndef GUI_COLOR_GREEN
  #if(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565)
    #define GUI_COLOR_GREEN (0x07E0)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888)
    #define GUI_COLOR_GREEN (0x00FF00)
  #elif(GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888)
    #define GUI_COLOR_GREEN (0x00FF00)
  #endif
#endif

//橙色定义
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
*                                              枚举结构体
*********************************************************************************************************/
//字体定义
typedef enum
{
  GUI_FONT_ASCII_12, //6x12的ASCII码
  GUI_FONT_ASCII_16, //8x16的ASCII码
  GUI_FONT_ASCII_24, //12x24的ASCII码
}EnumGUIFont;

//图片格式定义
typedef enum
{
  GUI_IMAGE_TYPE_BMP,   //位图
  GUI_IMAGE_TYPE_JPEG,  //JPEG图片
  GUI_IMAGE_TYPE_PNG,   //PNG图片
  GUI_IMAGE_TYPE_GIF,   //GIF图片
  GUI_IMAGE_TYPE_CLEAR, //不显示图片，控件完全透明
  GUI_IMAGE_TYPE_NULL,  //无效值
}EnumGUIImageType;

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/


#endif
