/*********************************************************************************************************
* 模块名称：GUIKeyBoard.c
* 摘    要：键盘模块
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
#include "GUIKeyBoard.h"
#include "GUIPlatform.h"
#include "ButtonWidget.h"
#include "InfoWidget.h"
#include "TLILCD.h"
#include "BMP.h"
#include "JPEG.h"
#include "Common.h"
#include "stdio.h"
#include "Keyboard.h"
#include "Malloc.h"
#include "SysTick.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
//按键枚举
typedef enum
{
  GUI_BUTTON_ESC = 0,   // ESC，必须是从0开始
  GUI_BUTTON_K1,        // 1
  GUI_BUTTON_K2,        // 2
  GUI_BUTTON_K3,        // 3
  GUI_BUTTON_K4,        // 4
  GUI_BUTTON_K5,        // 5
  GUI_BUTTON_K6,        // 6
  GUI_BUTTON_K7,        // 7
  GUI_BUTTON_K8,        // 8
  GUI_BUTTON_K9,        // 9
  GUI_BUTTON_K0,        // 0
  GUI_BUTTON_MINUS,     // - _
  GUI_BUTTON_EQUAL,     // = +
  GUI_BUTTON_BACK,      // Backspace
  GUI_BUTTON_TAB,       // Tab
  GUI_BUTTON_Q,         // Q
  GUI_BUTTON_W,         // W
  GUI_BUTTON_E,         // E
  GUI_BUTTON_R,         // R
  GUI_BUTTON_T,         // T
  GUI_BUTTON_Y,         // Y
  GUI_BUTTON_U,         // U
  GUI_BUTTON_I,         // I
  GUI_BUTTON_O,         // O
  GUI_BUTTON_P,         // P
  GUI_BUTTON_LPARAN,    // [ {
  GUI_BUTTON_RPARAN,    // ] }
  GUI_BUTTON_SLASH,     // \ |
  GUI_BUTTON_CAPSLOCK,  // CapLock
  GUI_BUTTON_A,         // A
  GUI_BUTTON_S,         // S
  GUI_BUTTON_D,         // D
  GUI_BUTTON_F,         // F
  GUI_BUTTON_G,         // G
  GUI_BUTTON_H,         // H
  GUI_BUTTON_J,         // J
  GUI_BUTTON_K,         // K
  GUI_BUTTON_L,         // L
  GUI_BUTTON_SEMICOLON, // ; :semicolon
  GUI_BUTTON_QUOTATION, // ' "
  GUI_BUTTON_ENTER,     // Enter
  GUI_BUTTON_LSHIFT,    // left Shift
  GUI_BUTTON_Z,         // Z
  GUI_BUTTON_X,         // X
  GUI_BUTTON_C,         // C
  GUI_BUTTON_V,         // V
  GUI_BUTTON_B,         // B
  GUI_BUTTON_N,         // N
  GUI_BUTTON_M,         // M
  GUI_BUTTON_COMMA,     // , <
  GUI_BUTTON_PERIOD,    // . >
  GUI_BUTTON_QUESTION,  // / ?
  GUI_BUTTON_RSHIFT,    // right Shift
  GUI_BUTTON_LCTRL,     // left Ctrl
  GUI_BUTTON_WIN,       // Windows
  GUI_BUTTON_LALT,      // left Alt
  GUI_BUTTON_SPASEBAR,  // Spacebar
  GUI_BUTTON_RALT,      // Right Alt
  GUI_BUTTON_RCTRL,     // right Ctrl
  GUI_BUTTON_MAX,       // 按键数量
  GUI_BUTTON_NONE,      // 无效按键
}EnumGUIButtons;

//按键扫描结果
typedef struct
{
  u32 num;                               //按键按下数量计数
  EnumGUIButtons button[GUI_BUTTON_MAX]; //按键扫描结果储存的是第一个、第二个、第n个按键按下的键值
}StructGUIButtonResult;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructButtonWidget    s_arrButton[GUI_BUTTON_MAX]; //触摸按键
static StructGUIButtonResult s_structButtonPress;         //按键扫描结果

//键值转换，GUI传回来的键值与HID键值不一致，需要做转换
//GUI键值编号是从左往右，从上至下，从0开始编号
static u8 s_arrKeyTable[] = 
{
  //第一行
  KEYBOARD_ESC, KEYBOARD_1, KEYBOARD_2, KEYBOARD_3, KEYBOARD_4, KEYBOARD_5, KEYBOARD_6, KEYBOARD_7,
  KEYBOARD_8, KEYBOARD_9, KEYBOARD_0, KEYBOARD_MINUS, KEYBOARD_EQUAL, KEYBOARD_BACKSPACE,

  //第二行
  KEYBOARD_TAB, KEYBOARD_Q, KEYBOARD_W, KEYBOARD_E, KEYBOARD_R, KEYBOARD_T, KEYBOARD_Y, KEYBOARD_U, 
  KEYBOARD_I, KEYBOARD_O, KEYBOARD_P, KEYBOARD_LPARAN, KEYBOARD_RPARAN, KEYBOARD_SLASH,

  //第三行
  KEYBOARD_CAPSLOCK, KEYBOARD_A, KEYBOARD_S, KEYBOARD_D, KEYBOARD_F, KEYBOARD_G, KEYBOARD_H, KEYBOARD_J,
  KEYBOARD_K, KEYBOARD_L, KEYBOARD_SEMICOLON, KEYBOARD_QUOTATION, KEYBOARD_ENTER,

  //第四行
  KEYBOARD_LEFT_SHIFT, KEYBOARD_Z, KEYBOARD_X, KEYBOARD_C, KEYBOARD_V, KEYBOARD_B, KEYBOARD_N, KEYBOARD_M,
  KEYBOARD_COMMA, KEYBOARD_PERIOD, KEYBOARD_QUESTION, KEYBOARD_RIGHT_SHIFT,

  //第五行
  KEYBOARD_LEFT_CTRL, KEYBOARD_LEFT_WIN, KEYBOARD_LEFT_ALT, KEYBOARD_SPACEBAR, KEYBOARD_RIGHT_ALT, 
  KEYBOARD_RIGHT_CTRL,
};

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void ButtonPressCallback(void* button);     //按键回调函数
static StructGUIButtonResult* ScanGUIButton(void); //按键扫描

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ButtonPressCallback
* 函数功能：按键回调函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ButtonPressCallback(void* button)
{
  EnumGUIButtons i;

  //获取按键按下结果
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    if((StructButtonWidget*)button == &s_arrButton[i])
    {
      s_structButtonPress.button[s_structButtonPress.num++] = i;
    }
  }
}

/*********************************************************************************************************
* 函数名称：ScanGUIButton
* 函数功能：按键扫描
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static StructGUIButtonResult* ScanGUIButton(void)
{
  EnumGUIButtons i;

  //标记无按键按下
  s_structButtonPress.num = 0;
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    s_structButtonPress.button[i] = GUI_BUTTON_NONE;
  }

  //依次扫描所有按键，并将按键扫描结果放在s_structButtonPress中
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    ScanButtonWidget(&s_arrButton[i]);
  }

  return &s_structButtonPress;
}

/*********************************************************************************************************
* 函数名称：KeyProc
* 函数功能：按键处理函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、按下控制键（shift、ctrl和win）后记得发送空按键指令给电脑以取消控制键状态，否则电脑会误以为控制键一直
*             处于按下状态，进而严重干扰电脑正常键盘输入
*          2、GUI按键扫描检测到没有按键按下时也会调用一次此回调函数，用以方便用户清除控制键状态
*********************************************************************************************************/
static void KeyProc(StructGUIButtonResult* result)
{
  u8 i, funcKey, key[6];

  //初始化键值
  funcKey = 0;
  for(i = 0; i < 6; i++)
  {
    key[i] = 0;
  }

  //获取键值填入临时缓冲区
  for(i = 0; i < result->num; i++)
  {
    //左Ctrl键
    if(GUI_BUTTON_LCTRL == result->button[i])
    {
      funcKey = funcKey | SET_LEFT_CTRL;
    }

    //右Ctrl键
    else if(GUI_BUTTON_RCTRL == result->button[i])
    {
      funcKey = funcKey | SET_RIGHT_CTRL;
    }

    //左Shift键
    else if(GUI_BUTTON_LSHIFT == result->button[i])
    {
      funcKey = funcKey | SET_LEFT_SHIFT;
    }

    //右Shift键
    else if(GUI_BUTTON_RSHIFT == result->button[i])
    {
      funcKey = funcKey | SET_RIGHT_SHIFT;
    }

    //左Alt键
    else if(GUI_BUTTON_LALT == result->button[i])
    {
      funcKey = funcKey | SET_LEFT_ALT;
    }

    //右Alt键
    else if(GUI_BUTTON_RALT == result->button[i])
    {
      funcKey = funcKey | SET_RIGHT_ALT;
    }

    //Windows键
    else if(GUI_BUTTON_WIN == result->button[i])
    {
      funcKey = funcKey | SET_LEFT_WINDOWS;
    }

    //键值转换并保存到键值缓冲区
    if(GUI_BUTTON_NONE != result->button[i])
    {
      key[i] = s_arrKeyTable[result->button[i]];
    }

    //只获取前6个键值
    if(i >= 5)
    {
      break;
    }
  }
  
  //上报键值
  while(0 != SendKeyVal(funcKey, key[0], key[1], key[2], key[3], key[4], key[5]));
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUIKeyBoard
* 函数功能：创建键盘
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIKeyBoard(void)
{
  static StructJpegImage s_structPressImage45x45;
  static StructJpegImage s_structReleaseImage45x45;
  static StructJpegImage s_structPressImage70x45;
  static StructJpegImage s_structReleaseImage70x45;
  static StructJpegImage s_structPressImage96x45;
  static StructJpegImage s_structReleaseImage96x45;
  static StructJpegImage s_structPressImage123x45;
  static StructJpegImage s_structReleaseImage123x45;
  static StructJpegImage s_structPressImage416x45;
  static StructJpegImage s_structReleaseImage416x45;
  StructInfoWidget infoWidget;
  StructBmpImage infoBmp;

  //显示字符
  static const char* s_arrButtonText[GUI_BUTTON_MAX] = 
  {"ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "- -", "= +", "Back",   //第一行
   "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[ {", "] }", "\\ |",   //第二行
   "CapsLock", "A", "S", "D", "F", "G", "H", "J", "K", "L", "; :", "' \"", "Enter", //第三行
   "Shift", "Z", "X", "C", "V", "B", "N", "M", ", <", ". >", "/ ?", "Shift",        //第四行
   "Ctrl", "Win", "Alt", "", "Alt", "Ctrl"};                                        //第五行

  //控件宽度
  static const u16 s_arrButtonWidth[GUI_BUTTON_MAX] = 
  {
    45 , 45, 45, 45 , 45, 45, 45, 45, 45, 45, 45, 45 , 45, 96, //第一行
    70 , 45, 45, 45 , 45, 45, 45, 45, 45, 45, 45, 45 , 45, 70, //第二行
    96 , 45, 45, 45 , 45, 45, 45, 45, 45, 45, 45, 45 , 96,     //第三行
    123, 45, 45, 45 , 45, 45, 45, 45, 45, 45, 45, 123,         //第四行
    96 , 45, 45, 416, 45, 96};                                 //第五行
  
  //字符横坐标相对于控件的偏移量
  static const u8 s_arrStringX0[GUI_BUTTON_MAX] = 
  {
    4  , 16, 16, 16 , 16, 16, 16, 16, 16, 16, 16, 4 , 4 , 24, //第一行
    17 , 16, 16, 16 , 16, 16, 16, 16, 16, 16, 16, 4 , 4 , 17, //第二行
    0  , 16, 16, 16 , 16, 16, 16, 16, 16, 16, 4 , 4 , 18,     //第三行
    31 , 16, 16, 16 , 16, 16, 16, 16, 4 , 4 , 4 , 31,         //第四行
    24 , 4 , 4 , 0  , 4 , 24};                                //第五行

  EnumGUIButtons i;
  u32 x, y;

  //LCD横屏显示
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //绘制背景
  DisPlayBMPByIPA(0, 0, MAIN_WINDOWN_BACKGROUND_DIR);

  s_structPressImage45x45.image = (unsigned char*)KEYBOARD_KEY_45x45_PRESS_DIR;
  s_structReleaseImage45x45.image = (unsigned char*)KEYBOARD_KEY_45x45_RELEASE_DIR;
  s_structPressImage70x45.image = (unsigned char*)KEYBOARD_KEY_70x45_PRESS_DIR;
  s_structReleaseImage70x45.image = (unsigned char*)KEYBOARD_KEY_70x45_RELEASE_DIR;
  s_structPressImage96x45.image = (unsigned char*)KEYBOARD_KEY_96x45_PRESS_DIR;
  s_structReleaseImage96x45.image = (unsigned char*)KEYBOARD_KEY_96x45_RELEASE_DIR;
  s_structPressImage123x45.image = (unsigned char*)KEYBOARD_KEY_123x45_PRESS_DIR;
  s_structReleaseImage123x45.image = (unsigned char*)KEYBOARD_KEY_123x45_RELEASE_DIR;
  s_structPressImage416x45.image = (unsigned char*)KEYBOARD_KEY_416x45_PRESS_DIR;
  s_structReleaseImage416x45.image = (unsigned char*)KEYBOARD_KEY_416x45_RELEASE_DIR;

  //标记无按键按下
  s_structButtonPress.num = 0;
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    s_structButtonPress.button[i] = GUI_BUTTON_NONE;
  }

  //创建按键
  x = 6;
  y = 130;
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    InitButtonWidgetStruct(&s_arrButton[i]);
    s_arrButton[i].x0 = x;
    s_arrButton[i].y0 = y;
    s_arrButton[i].width = s_arrButtonWidth[i];
    s_arrButton[i].height = 45;
    s_arrButton[i].textX0 = s_arrStringX0[i];
    s_arrButton[i].textY0 = 10;
    // s_arrButton[i].textColor = GUI_COLOR_ORANGE;
    s_arrButton[i].text = s_arrButtonText[i];
    s_arrButton[i].type = BUTTON_WIDGET_TYPE_LEVEL;
    s_arrButton[i].pressCallback = ButtonPressCallback;

    //设置背景图片
    s_arrButton[i].pressImageType = GUI_IMAGE_TYPE_JPEG;
    s_arrButton[i].releaseImageType = GUI_IMAGE_TYPE_JPEG;
    if(45 == s_arrButton[i].width)
    {
      s_arrButton[i].pressBackgroudImage = (void*)(&s_structPressImage45x45);
      s_arrButton[i].releaseBackgroudImage = (void*)(&s_structReleaseImage45x45);
    }
    else if(70 == s_arrButton[i].width)
    {
      s_arrButton[i].pressBackgroudImage = (void*)(&s_structPressImage70x45);
      s_arrButton[i].releaseBackgroudImage = (void*)(&s_structReleaseImage70x45);
    }
    else if(96 == s_arrButton[i].width)
    {
      s_arrButton[i].pressBackgroudImage = (void*)(&s_structPressImage96x45);
      s_arrButton[i].releaseBackgroudImage = (void*)(&s_structReleaseImage96x45);
    }
    else if(123 == s_arrButton[i].width)
    {
      s_arrButton[i].pressBackgroudImage = (void*)(&s_structPressImage123x45);
      s_arrButton[i].releaseBackgroudImage = (void*)(&s_structReleaseImage123x45);
    }
    else if(416 == s_arrButton[i].width)
    {
      s_arrButton[i].pressBackgroudImage = (void*)(&s_structPressImage416x45);
      s_arrButton[i].releaseBackgroudImage = (void*)(&s_structReleaseImage416x45);
    }

    //创建控件
    CreateButtonWidget(&s_arrButton[i]);

    //横坐标递增
    x = x + s_arrButton[i].width + 8;

    //切换到下一行
    if((GUI_BUTTON_BACK == i) || (GUI_BUTTON_SLASH == i) || (GUI_BUTTON_ENTER == i) || (GUI_BUTTON_RSHIFT == i) || (GUI_BUTTON_RCTRL == i))
    {
      x = 6;
      y = y + s_arrButton[0].height + 10;
    }
  }
  
  //初始化USB键盘驱动
  InitKeyboard();

  //等待配置完成
  infoBmp.alphaType = BMP_UES_ALPHA;
  infoBmp.storageType = BMP_IN_FATFS;
  infoBmp.addr = (void*)INFO_BACKGROUND_DIR;
  InitInfoWidgetStruct(&infoWidget);
  infoWidget.width = 300;
  infoWidget.height = 193;
  infoWidget.x0 = (g_structTLILCDDev.width[g_structTLILCDDev.currentLayer] - infoWidget.width) / 2;
  infoWidget.y0 = (g_structTLILCDDev.height[g_structTLILCDDev.currentLayer] - infoWidget.height) / 2;
  infoWidget.background = MyMalloc(SRAMEX, infoWidget.width * infoWidget.height * 2);
  infoWidget.image = (void*)&infoBmp;
  infoWidget.imageType = GUI_IMAGE_TYPE_BMP;
  CreateInfoWidget(&infoWidget);
  while(!IsKeyboardConfigFinish())
  {
    if(0 == infoWidget.isShow)
    {
      SetInfoWidgetShow(&infoWidget, "请重新插拔USB");
      DelayNms(500);
    }
  }
  DeleteInfoWidget(&infoWidget);
  MyFree(SRAMEX, infoWidget.background);
}

/*********************************************************************************************************
* 函数名称：DeleteGUIKeyBoard
* 函数功能：删除键盘
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUIKeyBoard(void)
{
  DeInitKeyboard();
}

/*********************************************************************************************************
* 函数名称：GUIKeyBoardPoll
* 函数功能：键盘轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIKeyBoardPoll(void)
{
  static u64 s_iSysLastTime = 0;
  static u64 s_iSysCurrentTime = 0;
  static u32 s_iLastKeyNum = 0;
  StructGUIButtonResult* button;

  //获取当前时间
  s_iSysCurrentTime = GUIGetSysTime();

  if((s_iSysCurrentTime - s_iSysLastTime) >= KEY_BOARD_SCAN_TIME)
  {
    s_iSysLastTime = s_iSysCurrentTime;

    //按键扫描
    button = ScanGUIButton();
    if((0 != button->num) || (s_iLastKeyNum != button->num))
    {
      KeyProc(button);
    }

    s_iLastKeyNum = button->num;
  }
}
