/*********************************************************************************************************
* 模块名称：GUITop.c
* 摘    要：照相机模块
* 当前版本：1.0.0
* 作    者：Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* 完成日期：2021年07月01日
* 内    容：
* 注    意：
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "GUITop.h"
#include "GUIPlatform.h"
#include "RadioWidget.h"
#include "ButtonWidget.h"
#include "TLILCD.h"
#include "BMP.h"
#include "JPEG.h"
#include "stdio.h"
#include "OV2640.h"
#include "DCI.h"
#include "Malloc.h"
#include "Beep.h"
#include "BMPEncoder.h"
#include "GUIRadio.h"
#include "GUIButton.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//字符串显示相关
#define STRING_MAX_LEN (64)  //显示字符最大长度
#define STRIGN_X0      (105) //显示字符横坐标起点
#define STRING_Y0      (747) //显示字符纵坐标起点
#define STRING_X1      (373) //显示字符横坐标终点
#define STRING_SIZE    (24)  //字体大小

//照片相关
#define IMAGE_X0       (116)
#define IMAGE_Y0       (195)
#define IMAGE_WIDTH    (240)
#define IMAGE_HEIGHT   (320)
#define IMAGE_SIZE     (IMAGE_WIDTH * IMAGE_HEIGHT * 2)

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructGUIDev* 			s_pGUIDev 					= NULL;
static u8*                s_pStringBackground = NULL;           //字符串显示背景缓冲区
static EnumGUIRadio       s_enumRadio         = GUI_RADIO_NONE; //当前选中的控件

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：DisplayBackground
* 函数功能：绘制背景
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：调用之前要确保LCD方向为横屏
*********************************************************************************************************/
static void DisplayBackground(void)
{
  //背景图片控制结构体
  StructJpegImage backgroundImage;

  //初始化backgroundImage
  backgroundImage.image = (unsigned char*)s_arrJpegBackgroundImage;
  backgroundImage.size  = sizeof(s_arrJpegBackgroundImage) / sizeof(unsigned char);

  //解码并显示图片
  DisplayJPEGInFlash(&backgroundImage, 0, 0);
}
/*********************************************************************************************************
* 函数名称：UpdataString
* 函数功能：更新字符串显示
* 输入参数：text：字符串
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void UpdataString(char* text)
{
  u32 len, max, total, begin;

  //计算字符串长度
  len = 0;
  while((0 != text[len]) && ('\r' != text[len]) && ('\n' != text[len]))
  {
    len++;
  }

  //计算屏幕一行最多显示多少个字符
  max = (STRING_X1 - STRIGN_X0) / 12;

  //所需的像素点总数
  total = 12 * len;

  //计算起始显示像素点
  begin = STRIGN_X0 + ((STRING_X1 - STRIGN_X0) - total) / 2;

  //绘制背景图片
  if(NULL != s_pStringBackground)
  {
    GUIDrawBackground(STRIGN_X0, STRING_Y0, STRING_X1 - STRIGN_X0, STRING_SIZE, (u32)s_pStringBackground);
  }
  
  //显示
  GUIDrawTextLine(begin, STRING_Y0, (u32)text, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1, max);
}
/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/

/*********************************************************************************************************
* 函数名称：InitGUI
* 函数功能：初始化GUI
* 输入参数：dev：GUI设备结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：书名使用12x24字体
*********************************************************************************************************/
void InitGUI(StructGUIDev* dev)
{

  //LCD竖屏显示
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //绘制背景
  DisplayBackground();
	  
	//创建单选模块
  InitGUIRadio();

  //创建按键
  InitGUIButton();
	
	//保存设备指针
  s_pGUIDev = dev;
	
	//默认无单选控件选中
  s_enumRadio = GUI_RADIO_NONE;

  //图像显示区域填充为黑色
  GUIFillColor(IMAGE_X0, IMAGE_Y0, IMAGE_X0 + IMAGE_WIDTH - 1, IMAGE_Y0 + IMAGE_HEIGHT - 1, GUI_COLOR_BLACK);

  //为字符串背景申请内存
  s_pStringBackground = MyMalloc(SDRAMEX, (STRING_X1 - STRIGN_X0) * STRING_SIZE * 2);
  if(NULL == s_pStringBackground)
  {
    while(1);
  }
	
  //保存字符串背景
  GUISaveBackground(STRIGN_X0, STRING_Y0, STRING_X1 - STRIGN_X0, STRING_SIZE, (u32)s_pStringBackground);
	
	//连接字符串更新函数指针
  s_pGUIDev->showInfo = UpdataString;
	
	//清空显示
  UpdataString("");
	
}


/*********************************************************************************************************
* 函数名称：GUITask
* 函数功能：照相机轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUITask()
{
  EnumGUIRadio radio;
  EnumGUIButtons button;

  //单选控件扫描
  radio = ScanGUIRadio();
  if(s_enumRadio != radio)
  {
    s_enumRadio = radio;
    if(NULL != s_pGUIDev->radioChangeCallback)
    {
      s_pGUIDev->radioChangeCallback(s_enumRadio);
    }
    return;
  }

  //按键扫描
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //上一项
    if(GUI_BUTTON_PREVIOUS == button)
    {
      if((NULL != s_pGUIDev->previousCallback) && (GUI_RADIO_NONE != s_enumRadio))
      {
        s_pGUIDev->previousCallback(s_enumRadio);
      }
    }

    //下一项
    else if((GUI_BUTTON_NEXT == button) && (GUI_RADIO_NONE != s_enumRadio))
    {
      if(NULL != s_pGUIDev->nextCallback)
      {
        s_pGUIDev->nextCallback(s_enumRadio);
      }
    }

    //拍照
    else if(GUI_BUTTON_PHOTO == button)
    {
      if(NULL != s_pGUIDev->takePhotoCallback)
      {
        s_pGUIDev->takePhotoCallback();
      }
    }
  }
}
