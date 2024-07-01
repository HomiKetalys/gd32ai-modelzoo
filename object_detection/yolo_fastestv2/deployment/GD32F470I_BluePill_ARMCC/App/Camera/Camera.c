/*********************************************************************************************************
* 模块名称：Camera.c
* 摘    要：摄像头模块
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
#include "Camera.h"
#include "GUITop.h"
#include "stdio.h"
#include "TLILCD.h"
#include "OV2640.h"
#include "SysTick.h"
#include "BMPEncoder.h"
#include "Beep.h"
#include "DCI.h"
#include "RadioWidget.h"
#include "ButtonWidget.h"
#include "FontLib.h"
#include "Malloc.h"

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

#define IMAGE_SIZE     (IMAGE_WIDTH * IMAGE_HEIGHT * 2)

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static u32                s_iImageFrameAdd    = NULL;           //图像帧首地址
//static StructRadioWidget  s_arrRadio[GUI_RADIO_MAX];            //单选控件

static u8*                s_pStringBackground = NULL;           //字符串显示背景缓冲区
//static EnumGUIRadio       s_enumRadio         = GUI_RADIO_NONE; //当前选中的控件

static StructGUIDev s_structGUIDev;     //GUI设备结构体
static char               s_arrStringBuf[64];                   //字符串转换缓冲区

//6种光照模式（白平衡）
const char* s_arrLmodeTable[5]  = {"自动", "晴天", "多云", "办公室", "家里"};

//7种特效
const char* s_arrEffectTable[7] = {"正常", "负片", "黑白", "偏红", "偏绿", "偏蓝", "复古"};

//摄像头参数
static i32 s_iLightMode       = 0; //白平衡（0-4）
static i32 s_iColorSaturation = 0; //色度（-2~+2）
static i32 s_iBrightness      = 0; //亮度（-2~+2）
static i32 s_iContrast        = 0; //对比度（-2~+2）
static i32 s_iEffects         = 0; //特效（0-6）

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void PreviouCallback(EnumGUIRadio item);     //前一项回调函数
static void NextCallback(EnumGUIRadio item);        //下一项回调函数
static void RadioChangeCallback(EnumGUIRadio item); //单选切换回调函数
static void TakePhotoCallback(void);                //拍照回调函数
/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
* 函数名称：RadioChange
* 函数功能：单选切换更新字符串显示
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void RadioChange(EnumGUIRadio item)
{
  //切换到色度调节
  if(GUI_RADIO_COLOR == item)
  {
    sprintf(s_arrStringBuf, "色度：%+d", s_iColorSaturation);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //切换到亮度调节
  else if(GUI_RADIO_LIGHT == item)
  {
    sprintf(s_arrStringBuf, "亮度：%+d", s_iBrightness);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //切换到对比度调节
  else if(GUI_RADIO_CONTRAST == item)
  {
    sprintf(s_arrStringBuf, "对比度：%+d", s_iContrast);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //切换到白平衡调节
  else if(GUI_RADIO_WB == item)
  {
    sprintf(s_arrStringBuf, "白平衡：%s", s_arrLmodeTable[s_iLightMode]);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //切换到特效调节
  else if(GUI_RADIO_EFECT == item)
  {
    sprintf(s_arrStringBuf, "特效：%s", s_arrEffectTable[s_iEffects]);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //清空显示
  else
  {
    s_structGUIDev.showInfo("");
  }
}

/*********************************************************************************************************
* 函数名称：PreviouCallback
* 函数功能：前一项回调函数
* 输入参数：button：控件首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：上一个默认递减，下一个默认递增
*********************************************************************************************************/
static void PreviouCallback(EnumGUIRadio button)
{
  //色度调节
  if(GUI_RADIO_COLOR == button)
  {
    if(s_iColorSaturation > -2)
    {
      s_iColorSaturation = s_iColorSaturation - 1;
    }
    else
    {
      s_iColorSaturation = +2;
    }
    sprintf(s_arrStringBuf, "色度：%+d", s_iColorSaturation);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640ColorSaturation(s_iColorSaturation);
  }

  //亮度调节
  else if(GUI_RADIO_LIGHT == button)
  {
    if(s_iBrightness > -2)
    {
      s_iBrightness = s_iBrightness - 1;
    }
    else
    {
      s_iBrightness = +2;
    }
    sprintf(s_arrStringBuf, "亮度：%+d", s_iBrightness);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640Brightness(s_iBrightness);
  }

  //对比度调节
  else if(GUI_RADIO_CONTRAST == button)
  {
    if(s_iContrast > -2)
    {
      s_iContrast = s_iContrast - 1;
    }
    else
    {
      s_iContrast = +2;
    }
    sprintf(s_arrStringBuf, "对比度：%+d", s_iContrast);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640Contrast(s_iContrast);
  }

  //白平衡调节
  else if(GUI_RADIO_WB == button)
  {
    if(s_iLightMode > 0)
    {
      s_iLightMode = s_iLightMode - 1;
    }
    else
    {
      s_iLightMode = 4;
    }
    sprintf(s_arrStringBuf, "白平衡：%s", s_arrLmodeTable[s_iLightMode]);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640LightMode(s_iLightMode);
  }

  //特效调节
  else if(GUI_RADIO_EFECT == button)
  {
    if(s_iEffects > 0)
    {
      s_iEffects = s_iEffects - 1;
    }
    else
    {
      s_iEffects = 6;
    }
    sprintf(s_arrStringBuf, "特效：%s", s_arrEffectTable[s_iEffects]);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640SpecialEffects(s_iEffects);
  }
}

/*********************************************************************************************************
* 函数名称：NextCallback
* 函数功能：下一项回调函数
* 输入参数：button：控件首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：上一个默认递减，下一个默认递增
*********************************************************************************************************/
static void NextCallback(EnumGUIRadio button)
{
  //色度调节
  if(GUI_RADIO_COLOR == button)
  {
    if(s_iColorSaturation < 2)
    {
      s_iColorSaturation = s_iColorSaturation + 1;
    }
    else
    {
      s_iColorSaturation = -2;
    }
    sprintf(s_arrStringBuf, "色度：%+d", s_iColorSaturation);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640ColorSaturation(s_iColorSaturation);
  }

  //亮度调节
  else if(GUI_RADIO_LIGHT == button)
  {
    if(s_iBrightness < 2)
    {
      s_iBrightness = s_iBrightness + 1;
    }
    else
    {
      s_iBrightness = -2;
    }
    sprintf(s_arrStringBuf, "亮度：%+d", s_iBrightness);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640Brightness(s_iBrightness);
  }

  //对比度调节
  else if(GUI_RADIO_CONTRAST == button)
  {
    if(s_iContrast < 2)
    {
      s_iContrast = s_iContrast + 1;
    }
    else
    {
      s_iContrast = -2;
    }
    sprintf(s_arrStringBuf, "对比度：%+d", s_iContrast);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640Contrast(s_iContrast);
  }

  //白平衡调节
  else if(GUI_RADIO_WB == button)
  {
    if(s_iLightMode < 4)
    {
      s_iLightMode = s_iLightMode + 1;
    }
    else
    {
      s_iLightMode = 0;
    }
    sprintf(s_arrStringBuf, "白平衡：%s", s_arrLmodeTable[s_iLightMode]);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640LightMode(s_iLightMode);
  }

  //特效调节
  else if(GUI_RADIO_EFECT == button)
  {
    if(s_iEffects < 6)
    {
      s_iEffects = s_iEffects + 1;
    }
    else
    {
      s_iEffects = 0;
    }
    sprintf(s_arrStringBuf, "特效：%s", s_arrEffectTable[s_iEffects]);
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640SpecialEffects(s_iEffects);
  }
}

/*********************************************************************************************************
* 函数名称：TakePhotoCallback
* 函数功能：拍照回调函数
* 输入参数：button：控件首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void TakePhotoCallback(void)
{
  //蜂鸣器响起100ms
  BeepWithTime(100);

  //截取屏幕图像并保存到SD卡
	ScreeShot();
}
/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitCamera
* 函数功能：初始化摄像头模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitCamera(void)
{
	//初始化摄像头参数
  s_iLightMode       = 0;
  s_iColorSaturation = 0;
  s_iBrightness      = 0;
  s_iContrast        = 0;
  s_iEffects         = 0;
	
	//初始化摄像头
  if(InitOV2640() == 0)
  {
		//配置OV2640
		DelayNms(50);
		OV2640RGB565Mode();
		OV2640OutSizeSet(IMAGE_WIDTH, IMAGE_HEIGHT);
		OV2640AutoExposure(0);
		OV2640LightMode(s_iLightMode);
		OV2640ColorSaturation(s_iColorSaturation);
		OV2640Brightness(s_iBrightness);
		OV2640Contrast(s_iContrast);
		OV2640SpecialEffects(s_iEffects);

		//设置图像显存地址为前景层首地址
		s_iImageFrameAdd = g_structTLILCDDev.foreFrameAddr;
		//配置DCI
		InitDCI(s_iImageFrameAdd, IMAGE_SIZE, IMAGE_X0, IMAGE_Y0, IMAGE_WIDTH, IMAGE_HEIGHT, 0);
  }
  else
  {
    LCDShowString(30, 210, 200, 16, LCD_FONT_16,LCD_TEXT_NORMAL,LCD_COLOR_BLUE,LCD_COLOR_BLACK,"OV2640 Error!!");
    while(1);
  }
	
  //为字符串背景申请内存
  s_pStringBackground = MyMalloc(SDRAMEX, (STRING_X1 - STRIGN_X0) * STRING_SIZE * 2);
  if(NULL == s_pStringBackground)
  {
    while(1);
  }
	
	//设置回调函数
  s_structGUIDev.previousCallback    = PreviouCallback;
  s_structGUIDev.nextCallback        = NextCallback;
  s_structGUIDev.radioChangeCallback = RadioChange;
  s_structGUIDev.takePhotoCallback   = TakePhotoCallback;
  
  InitGUI(&s_structGUIDev);
}

/*********************************************************************************************************
* 函数名称：CameraTask
* 函数功能：摄像头模块任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CameraTask(void)
{
  //GUI任务
  GUITask();

  //显示一帧图形到屏幕上
  DCIShowImage();
}
