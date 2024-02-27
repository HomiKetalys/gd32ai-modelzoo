/*********************************************************************************************************
* 模块名称：GUICamera.c
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
#include "GUICamera.h"
#include "GUIPlatform.h"
#include "RadioWidget.h"
#include "ButtonWidget.h"
#include "TLILCD.h"
#include "BMP.h"
#include "Common.h"
#include "stdio.h"
#include "OV2640.h"
#include "DCI.h"
#include "Malloc.h"
#include "Beep.h"
#include "BMPEncoder.h"

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
*                                              枚举结构体定义
*********************************************************************************************************/
typedef enum
{
  GUI_RADIO_COLOR = 0, //色度，必须是从0开始
  GUI_RADIO_LIGHT,     //亮度
  GUI_RADIO_CONTRAST,  //对比度
  GUI_RADIO_WB,        //白平衡
  GUI_RADIO_EFECT,     //特效
  GUI_RADIO_MAX,       //控件数量
  GUI_RADIO_NONE,      //无效控件
}EnumGUIRadio;

typedef enum
{
  GUI_BUTTON_PREVIOUS = 0, //上一个按键，必须是从0开始
  GUI_BUTTON_NEXT,         //下一个按键
  GUI_BUTTON_PHOTO,        //拍照按键
  GUI_BUTTON_MAX,          //按键数量
  GUI_BUTTON_NONE,         //无效按键
}EnumGUIButtons;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructRadioWidget  s_arrRadio[GUI_RADIO_MAX];            //单选控件
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX];          //触摸按键
static u32                s_iImageFrameAdd    = NULL;           //图像帧首地址
static u8*                s_pStringBackground = NULL;           //字符串显示背景缓冲区
static EnumGUIRadio       s_enumRadio         = GUI_RADIO_NONE; //当前选中的控件
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

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：RadioPressCallback
* 函数功能：单选回调函数
* 输入参数：radio：被按下的单选控件地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：有且仅有一个单选控件能获取焦点
*********************************************************************************************************/
static void RadioPressCallback(void* radio)
{
  u8 i;
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    if((StructRadioWidget*)radio != &s_arrRadio[i])
    {
      //取消控件焦点状态
      ClearRadioWidgetFocus(&s_arrRadio[i]);
    }
  }
}

/*********************************************************************************************************
* 函数名称：ScanGUIRadio
* 函数功能：单选模块扫描
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static EnumGUIRadio ScanGUIRadio(void)
{
  u32 i;
  EnumGUIRadio result;

  //扫描
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    ScanRadioWidget(&s_arrRadio[i]);
  }

  //获取具有焦点的控件ID
  result = GUI_RADIO_NONE;
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    if(RADIO_HAS_FOCUS == s_arrRadio[i].hasFocus)
    {
      result = (EnumGUIRadio)i;
      break;
    }
  }

  //返回
  return result;
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
* 函数名称：PreviouCallback
* 函数功能：前一项回调函数
* 输入参数：button：控件首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：上一个默认递减，下一个默认递增
*********************************************************************************************************/
static void PreviouCallback(void* button)
{
  //色度调节
  if(GUI_RADIO_COLOR == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640ColorSaturation(s_iColorSaturation);
  }

  //亮度调节
  else if(GUI_RADIO_LIGHT == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640Brightness(s_iBrightness);
  }

  //对比度调节
  else if(GUI_RADIO_CONTRAST == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640Contrast(s_iContrast);
  }

  //白平衡调节
  else if(GUI_RADIO_WB == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640LightMode(s_iLightMode);
  }

  //特效调节
  else if(GUI_RADIO_EFECT == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
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
static void NextCallback(void* button)
{
  //色度调节
  if(GUI_RADIO_COLOR == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640ColorSaturation(s_iColorSaturation);
  }

  //亮度调节
  else if(GUI_RADIO_LIGHT == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640Brightness(s_iBrightness);
  }

  //对比度调节
  else if(GUI_RADIO_CONTRAST == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640Contrast(s_iContrast);
  }

  //白平衡调节
  else if(GUI_RADIO_WB == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640LightMode(s_iLightMode);
  }

  //特效调节
  else if(GUI_RADIO_EFECT == s_enumRadio)
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
    UpdataString(s_arrStringBuf);
    OV2640SpecialEffects(s_iEffects);
  }
}

/*********************************************************************************************************
* 函数名称：RadioChange
* 函数功能：单选切换更新字符串显示
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void RadioChange(void)
{
  //切换到色度调节
  if(GUI_RADIO_COLOR == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "色度：%+d", s_iColorSaturation);
    UpdataString(s_arrStringBuf);
  }

  //切换到亮度调节
  else if(GUI_RADIO_LIGHT == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "亮度：%+d", s_iBrightness);
    UpdataString(s_arrStringBuf);
  }

  //切换到对比度调节
  else if(GUI_RADIO_CONTRAST == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "对比度：%+d", s_iContrast);
    UpdataString(s_arrStringBuf);
  }

  //切换到白平衡调节
  else if(GUI_RADIO_WB == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "白平衡：%s", s_arrLmodeTable[s_iLightMode]);
    UpdataString(s_arrStringBuf);
  }

  //切换到特效调节
  else if(GUI_RADIO_EFECT == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "特效：%s", s_arrEffectTable[s_iEffects]);
    UpdataString(s_arrStringBuf);
  }

  //清空显示
  else
  {
    UpdataString("");
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
static void TakePhotoCallback(void* button)
{
  //蜂鸣器响起100ms
  BeepWithTime(100);

  //截取屏幕图像并保存到SD卡
  BMPEncodeWithRGB565(IMAGE_X0, IMAGE_Y0, IMAGE_WIDTH, IMAGE_HEIGHT, "0:/photo", "OV2640Image");
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUICamera
* 函数功能：创建照相机
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：横屏时建议打开前景层，同时DCI使用模式0，DMA直接将DCI数据传输到现存中，刷新速度快
*           但LCD刷新率要调低，否则有可能会出现花屏
*********************************************************************************************************/
void CreateGUICamera(void)
{
  //单选控件坐标
  static const u16 s_arrX0[GUI_RADIO_MAX] = {43, 204, 365, 42 , 365};  //横坐标
  static const u16 s_arrY0[GUI_RADIO_MAX] = {17, 20 , 20  , 645, 645}; //纵坐标
  u32 i;

  //LCD竖屏显示
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //绘制背景
  DisPlayBMPByIPA(0, 0, CAMERA_BACKGROUND_DIR);

  //图像显示区域填充为黑色
  GUIFillColor(IMAGE_X0, IMAGE_Y0, IMAGE_X0 + IMAGE_WIDTH - 1, IMAGE_Y0 + IMAGE_HEIGHT - 1, GUI_COLOR_BLACK);

  //创建上一项按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PREVIOUS]);
  s_arrButton[GUI_BUTTON_PREVIOUS].x0 = 52;
  s_arrButton[GUI_BUTTON_PREVIOUS].y0 = 733;
  s_arrButton[GUI_BUTTON_PREVIOUS].width = 53;
  s_arrButton[GUI_BUTTON_PREVIOUS].height = 53;
  s_arrButton[GUI_BUTTON_PREVIOUS].text = "";
  s_arrButton[GUI_BUTTON_PREVIOUS].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PREVIOUS].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PREVIOUS].pressCallback = PreviouCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PREVIOUS]);

  //创建下一项按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_NEXT]);
  s_arrButton[GUI_BUTTON_NEXT].x0 = 373;
  s_arrButton[GUI_BUTTON_NEXT].y0 = 733;
  s_arrButton[GUI_BUTTON_NEXT].width = 53;
  s_arrButton[GUI_BUTTON_NEXT].height = 53;
  s_arrButton[GUI_BUTTON_NEXT].text = "";
  s_arrButton[GUI_BUTTON_NEXT].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_NEXT].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_NEXT].pressCallback = NextCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_NEXT]);

  //创建拍照按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PHOTO]);
  s_arrButton[GUI_BUTTON_PHOTO].x0 = 183;
  s_arrButton[GUI_BUTTON_PHOTO].y0 = 622;
  s_arrButton[GUI_BUTTON_PHOTO].width = 115;
  s_arrButton[GUI_BUTTON_PHOTO].height = 115;
  s_arrButton[GUI_BUTTON_PHOTO].text = "";
  s_arrButton[GUI_BUTTON_PHOTO].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PHOTO].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PHOTO].pressCallback = TakePhotoCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PHOTO]);

  //创建单选控件
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    InitRadioWidgetStruct(&s_arrRadio[i]);
    s_arrRadio[i].x0 = s_arrX0[i];
    s_arrRadio[i].y0 = s_arrY0[i];
    s_arrRadio[i].width = 75;
    s_arrRadio[i].height = 75;
    s_arrRadio[i].type = RADIO_TYPE_LINE;
    s_arrRadio[i].lineColor = GUI_COLOR_WHITE;
    s_arrRadio[i].lineSize = 4;
    s_arrRadio[i].background = MyMalloc(SRAMEX, s_arrRadio[i].width * s_arrRadio[i].height * 2);
    s_arrRadio[i].pressCallback = RadioPressCallback;
    CreateRadioWidget(&s_arrRadio[i]);
  }

  //为字符串背景申请内存
  s_pStringBackground = MyMalloc(SRAMEX, (STRING_X1 - STRIGN_X0) * STRING_SIZE * 2);
  if(NULL == s_pStringBackground)
  {
    while(1);
  }

  //保存字符串背景
  GUISaveBackground(STRIGN_X0, STRING_Y0, STRING_X1 - STRIGN_X0, STRING_SIZE, (u32)s_pStringBackground);

  //配置OV2640
  InitOV2640();
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
  InitDCI(s_iImageFrameAdd, IMAGE_SIZE, IMAGE_X0, IMAGE_Y0, IMAGE_WIDTH, IMAGE_HEIGHT, 1);
}

/*********************************************************************************************************
* 函数名称：DeleteGUICamera
* 函数功能：删除照相机
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUICamera(void)
{
  u32 i;

  //复位DCI
  DeInitDCI();

  //摄像头进入掉电模式
  OV2640PWDNSet(1);

  //禁用前景层
  //LCDLayerDisable(LCD_LAYER_FOREGROUND);

  //释放单选控件申请的动态内存，并删除控件
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    MyFree(SRAMEX, s_arrRadio[i].background);
    DeleteRadioWidget(&s_arrRadio[i]);
  }

  //删除按键控件
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    DeleteButtonWidget(&s_arrButton[i]);
  }
}

/*********************************************************************************************************
* 函数名称：GUICameraPoll
* 函数功能：照相机轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUICameraPoll(void)
{
  EnumGUIRadio radio;
  u32 i;

  //显示一帧图形到屏幕上
  DCIShowImage();

  //单选控件扫描
  radio = ScanGUIRadio();
  if(s_enumRadio != radio)
  {
    s_enumRadio = radio;
    RadioChange();
  }

  //按键扫描
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    ScanButtonWidget(&s_arrButton[i]);
  }
}
