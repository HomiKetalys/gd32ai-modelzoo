/*********************************************************************************************************
* 模块名称：GUICAN.c
* 摘    要：GUI CAN通信模块
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
#include "GUICAN.h"
#include "GUIPlatform.h"
#include "ButtonWidget.h"
#include "LineEditWidget.h"
#include "TerminalWidget.h"
#include "BMP.h"
#include "JPEG.h"
#include "Common.h"
#include "TLILCD.h"
#include "CAN.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
typedef enum
{
  GUI_BUTTON_K0 = 0, //K0按键，必须是从0开始
  GUI_BUTTON_K1,     //K1按键
  GUI_BUTTON_K2,     //K2按键
  GUI_BUTTON_K3,     //K3按键
  GUI_BUTTON_K4,     //K4按键
  GUI_BUTTON_K5,     //K5按键
  GUI_BUTTON_K6,     //K6按键
  GUI_BUTTON_K7,     //K7按键
  GUI_BUTTON_K8,     //K8按键
  GUI_BUTTON_K9,     //K9按键
  GUI_BUTTON_KA,     //KA按键
  GUI_BUTTON_KB,     //KB按键
  GUI_BUTTON_KC,     //KC按键
  GUI_BUTTON_KD,     //KD按键
  GUI_BUTTON_KE,     //KE按键
  GUI_BUTTON_KF,     //KF按键
  GUI_BUTTON_SEND,   //发送按键
  GUI_BUTTON_BACK,   //撤回按键
  GUI_BUTTON_MAX,    //按键数量
  GUI_BUTTON_NONE,   //无效按键
}EnumGUIButtons;

typedef enum
{
  GUI_LINE_EDIT_SEND_ID = 0, //发送ID
  GUI_LINE_EDIT_SEND_DATA,   //发送数据，必须是从0开始
  GUI_LINE_EDIT_MAX,         //控件数量
  GUI_LINE_EDIT_NONE,        //无效控件
}EnumGUILineEdit;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
//按键相关
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX]; //触摸按键
static EnumGUIButtons     s_enumButtonPress;           //用来标记那个按键按下

//行编辑相关
static StructLineEditWidget s_arrLineEdit[GUI_LINE_EDIT_MAX];

//终端相关
static StructTerminalWidget s_structTerminal;

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void ButtonPressCallback(void* button);       //按键回调函数
static EnumGUIButtons ScanGUIButton(void);           //按键扫描
static void LineEditPressCallback(void* lineEdit);   //行编辑回调函数
static u32  CharToU32(char c);                       //字符转int
static void ScanGUILineEdit(void);                   //行编辑模块扫描
static u8   GetGUILineEditIDResult(u32* result);     //获取发送ID输入结果
static u8   GetGUILineEditDataResult(char** result); //获取发送ID输入结果
static void SendProc(u32 id, char* data);            //发送处理函数
static void ReadProc(void);                          //读串口处理

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

  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    if((StructButtonWidget*)button == &s_arrButton[i])
    {
      s_enumButtonPress = i;
      return;
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
* 注    意：每隔20毫秒执行一次
*********************************************************************************************************/
static EnumGUIButtons ScanGUIButton(void)
{
  EnumGUIButtons i;

  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    s_enumButtonPress = GUI_BUTTON_NONE;
    ScanButtonWidget(&s_arrButton[i]);

    if(GUI_BUTTON_NONE != s_enumButtonPress)
    {
      return s_enumButtonPress;
    }
  }

  return GUI_BUTTON_NONE;
}

/*********************************************************************************************************
* 函数名称：LineEditPressCallback
* 函数功能：行编辑回调函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：有且仅有一个行编辑控件能获取焦点
*********************************************************************************************************/
static void LineEditPressCallback(void* lineEdit)
{
  u8 i;
  for(i = 0; i < GUI_LINE_EDIT_MAX; i++)
  {
    if((StructLineEditWidget*)lineEdit != &s_arrLineEdit[i])
    {
      //取消控件焦点状态
      ClearLineEditWidgetFocus(&s_arrLineEdit[i]);
    }
  }
}

/*********************************************************************************************************
* 函数名称：CharToU32
* 函数功能：字符转int
* 输入参数：c：需要转换的字符
* 输出参数：void
* 返 回 值：字符对应的数值，返回16表示出错
* 创建日期：2021年07月01日
* 注    意：有且仅有一个行编辑控件能获取焦点
*********************************************************************************************************/
static u32 CharToU32(char c)
{
  u32 result;
  switch(c)
  {
    case '0': result = 0 ; break;
    case '1': result = 1 ; break;
    case '2': result = 2 ; break;
    case '3': result = 3 ; break;
    case '4': result = 4 ; break;
    case '5': result = 5 ; break;
    case '6': result = 6 ; break;
    case '7': result = 7 ; break;
    case '8': result = 8 ; break;
    case '9': result = 9 ; break;
    case 'A': result = 10; break;
    case 'a': result = 10; break;
    case 'B': result = 11; break;
    case 'b': result = 11; break;
    case 'C': result = 12; break;
    case 'c': result = 12; break;
    case 'D': result = 13; break;
    case 'd': result = 13; break;
    case 'E': result = 14; break;
    case 'e': result = 14; break;
    case 'F': result = 15; break;
    case 'f': result = 15; break;
    default : result = 16; break;
  }

  return result;
}

/*********************************************************************************************************
* 函数名称：ScanGUILineEdit
* 函数功能：行编辑模块扫描
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ScanGUILineEdit(void)
{
  u8 i;
  for(i = 0; i < GUI_LINE_EDIT_MAX; i++)
  {
    ScanLineEditWidget(&s_arrLineEdit[i]);
  }
}

/*********************************************************************************************************
* 函数名称：GetGUILineEditIDResult
* 函数功能：获取发送ID输入结果
* 输入参数：result：用于返回输入结果
* 输出参数：void
* 返 回 值：0-获取成功，其它：获取失败
* 创建日期：2021年07月01日
* 注    意：行编辑中为10进制数据
*********************************************************************************************************/
static u8 GetGUILineEditIDResult(u32* result)
{
  u32 sum;       //最终结果
  u32 charValue; //当前字符对应的数值
  u8 i = 0;

  //计算行编辑输入值
  i = 0;
  sum = 0;
  while(1)
  {
    charValue = CharToU32(s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].text[i]);
    if(charValue < 16)
    {
      sum = sum * 16 + charValue;
    }
    else
    {
      break;
    }

    i = i + 1;
  }

  //读取第一个字符对应的数值时失败，表示当前行编辑输入为空
  if(0 == i)
  {
    return 1;
  }

  //返回输入值
  else
  {
    *result = sum;
    return 0;
  }
}

/*********************************************************************************************************
* 函数名称：GetGUILineEditIDResult
* 函数功能：获取发送ID输入结果
* 输入参数：result：用于返回输入结果
* 输出参数：void
* 返 回 值：0-获取成功，其它：获取失败
* 创建日期：2021年07月01日
* 注    意：行编辑中为10进制数据
*********************************************************************************************************/
static u8 GetGUILineEditDataResult(char** result)
{
  //行编辑中有数据
  if(0 != s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].text[0])
  {
    *result = s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].text;
    return 0;
  }
  
  //无数据
  else
  {
    result = NULL;
    return 1;
  }
}

/*********************************************************************************************************
* 函数名称：SendProc
* 函数功能：发送处理函数
* 输入参数：id：发送ID，data：发送数据
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：因为键盘只有0-9，所以ID输入框是10进制数据，发送数据是字符串
*********************************************************************************************************/
static void SendProc(u32 id, char* data)
{
  u8 i;        //发送计数
  u8 sendData; //发送数据

  //校验输入地址是否为空指针
  if(NULL == data)
  {
    return;
  }

  //计算数据量
  i = 0;
  while(0 != *(data + i))
  {
    i++;
  }

  //通过CAN0将数据发送出去
  WriteCAN0(id, (u8*)data, i);

  //发送回车换行
  sendData = '\r';
  WriteCAN0(id, &sendData, 1);
  sendData = '\n';
  WriteCAN0(id, &sendData, 1);
}

/*********************************************************************************************************
* 函数名称：ReadProc
* 函数功能：读串口处理
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：每隔一段时间调用一次
*********************************************************************************************************/
static void ReadProc(void)
{
  static char s_arrReadBuf[48] = {0}; //接收缓冲区
  static u8   s_iReadCnt       = 0;   //接收计数
  static u64  s_iBeginTime     = 0;   //接收到第一个字符时的系统时间

  u8  readData;
  u64 time;

  //获取系统运行时间
  time = GUIGetSysTime();  //ms

  //接收数据处理
  while(ReadCAN0(&readData, 1))
  {
    //记录接收到第一个字符时的系统时间
    if(0 == s_iReadCnt)
    {
      s_iBeginTime = time;
    }

    //将数据储存到缓冲区
    s_arrReadBuf[s_iReadCnt] = readData;

    //接收计数加一
    s_iReadCnt = (s_iReadCnt + 1) % (sizeof(s_arrReadBuf) / sizeof(u8));

    //接收到回车换行
    if(s_iReadCnt >= 2)
    {
      if(('\r' == s_arrReadBuf[s_iReadCnt - 2]) && ('\n' == s_arrReadBuf[s_iReadCnt - 1]))
      {
        //加上字符串结尾
        s_arrReadBuf[s_iReadCnt] = 0;

        //接收计数清零
        s_iReadCnt = 0;

        //输出到终端显示
        TerminalWidgetShowLine(&s_structTerminal, s_arrReadBuf);
      }
    }
  }

  //超过250毫秒还没接收到回车换行则强制更新到终端显示
  if((s_iReadCnt > 0) && ((time - s_iBeginTime) > 250))
  {
    //加上字符串结尾
    s_arrReadBuf[s_iReadCnt] = 0;

    //接收计数清零
    s_iReadCnt = 0;

    //输出到终端显示
    TerminalWidgetShowLine(&s_structTerminal, s_arrReadBuf);
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUICAN
* 函数功能：创建GUI CAN界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUICAN(void)
{
  static const u16 s_arrButtonX[GUI_BUTTON_MAX] = {485, 560, 635, 710, //K0-K3
                                                   485, 560, 635, 710, //K4-K7
                                                   485, 560, 635, 710, //K8-KB
                                                   485, 560, 635, 710, //KC-KF
                                                   485, 635};          //Send, Back

  static const u16 s_arrButtonY[GUI_BUTTON_MAX] = {105, 105, 105, 105, //K0-K3
                                                   180, 180, 180, 180, //K4-K7
                                                   255, 255, 255, 255, //K8-KB
                                                   330, 330, 330, 330, //KC-KF
                                                   405, 405};          //Send, Back
  static const char* s_arrButtonText[16] = {"0","1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
  static StructBmpImage s_structKeyNumPressImage;
  static StructBmpImage s_structKeyNumReleaseImage;
  static StructBmpImage s_structKeyFucPressImage;
  static StructBmpImage s_structKeyFucReleaseImage;
  static StructJpegImage s_structHasFocusImage;
  static StructJpegImage s_structNoFocusImage;
  static StructJpegImage s_structTerminalImage;

  EnumGUIButtons i;

  //LCD横屏显示
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //显示背景图片
  DisPlayBMPByIPA(0, 0, CAN_BACKGROUND_DIR);

  s_structKeyNumPressImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyNumPressImage.alphaGate   = 1;
  s_structKeyNumPressImage.storageType = BMP_IN_FATFS;
  s_structKeyNumPressImage.addr        = (void*)CAN_BUTTON_PRESS_65x65_DIR;

  s_structKeyNumReleaseImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyNumReleaseImage.alphaGate   = 1;
  s_structKeyNumReleaseImage.storageType = BMP_IN_FATFS;
  s_structKeyNumReleaseImage.addr        = (void*)CAN_BUTTON_RELEASE_65x65_DIR;

  s_structKeyFucPressImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyFucPressImage.alphaGate   = 1;
  s_structKeyFucPressImage.storageType = BMP_IN_FATFS;
  s_structKeyFucPressImage.addr        = (void*)CAN_BUTTON_PRESS_140x65_DIR;

  s_structKeyFucReleaseImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyFucReleaseImage.alphaGate   = 1;
  s_structKeyFucReleaseImage.storageType = BMP_IN_FATFS;
  s_structKeyFucReleaseImage.addr        = (void*)CAN_BUTTON_RELEASE_140x65_DIR;

  //标记无按键按下
  s_enumButtonPress = GUI_BUTTON_NONE;

  //创建K0-KF
  for(i = GUI_BUTTON_K0; i <= GUI_BUTTON_KF; i++)
  {
    InitButtonWidgetStruct(&s_arrButton[i]);
    s_arrButton[i].x0 = s_arrButtonX[i];
    s_arrButton[i].y0 = s_arrButtonY[i];
    s_arrButton[i].width = 65;
    s_arrButton[i].height = 65;
    s_arrButton[i].textX0 = 25;
    s_arrButton[i].textY0 = 20;
    s_arrButton[i].text = s_arrButtonText[i];
    s_arrButton[i].pressBackgroudImage = (void*)(&s_structKeyNumPressImage);
    s_arrButton[i].releaseBackgroudImage = (void*)(&s_structKeyNumReleaseImage);
    s_arrButton[i].pressCallback = ButtonPressCallback;
    CreateButtonWidget(&s_arrButton[i]);
  }

  //创建读取按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_SEND]);
  s_arrButton[GUI_BUTTON_SEND].x0 = s_arrButtonX[GUI_BUTTON_SEND];
  s_arrButton[GUI_BUTTON_SEND].y0 = s_arrButtonY[GUI_BUTTON_SEND];
  s_arrButton[GUI_BUTTON_SEND].width = 140;
  s_arrButton[GUI_BUTTON_SEND].height = 65;
  s_arrButton[GUI_BUTTON_SEND].textX0 = 46;
  s_arrButton[GUI_BUTTON_SEND].textY0 = 20;
  s_arrButton[GUI_BUTTON_SEND].text = "SEND";
  s_arrButton[GUI_BUTTON_SEND].pressBackgroudImage = (void*)(&s_structKeyFucPressImage);
  s_arrButton[GUI_BUTTON_SEND].releaseBackgroudImage = (void*)(&s_structKeyFucReleaseImage);
  s_arrButton[GUI_BUTTON_SEND].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_SEND]);

  //创建撤回按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_BACK]);
  s_arrButton[GUI_BUTTON_BACK].x0 = s_arrButtonX[GUI_BUTTON_BACK];
  s_arrButton[GUI_BUTTON_BACK].y0 = s_arrButtonY[GUI_BUTTON_BACK];
  s_arrButton[GUI_BUTTON_BACK].width = 140;
  s_arrButton[GUI_BUTTON_BACK].height = 65;
  s_arrButton[GUI_BUTTON_BACK].textX0 = 46;
  s_arrButton[GUI_BUTTON_BACK].textY0 = 20;
  s_arrButton[GUI_BUTTON_BACK].text = "BACK";
  s_arrButton[GUI_BUTTON_BACK].pressBackgroudImage = (void*)(&s_structKeyFucPressImage);
  s_arrButton[GUI_BUTTON_BACK].releaseBackgroudImage = (void*)(&s_structKeyFucReleaseImage);
  s_arrButton[GUI_BUTTON_BACK].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_BACK]);

  //初始化JPEG图片
  s_structHasFocusImage.image = (unsigned char*)CAN_LINE_EDIT_HAS_FOCUS_DIR;
  s_structNoFocusImage.image = (unsigned char*)CAN_LINE_EDIT_NO_FOCUS_DIR;

  //发送ID
  InitLineEditWidgetStruct(&s_arrLineEdit[GUI_LINE_EDIT_SEND_ID]);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].x0 = 25;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].y0 = 405;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].width = 200;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].height = 65;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].textX0 = 3;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].textY0 = 19;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].hasFocusBackgroundImage = (void*)(&s_structHasFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].noFocusBackgroundImage = (void*)(&s_structNoFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_ID].pressCallback = LineEditPressCallback;
  CreateLineEditWidget(&s_arrLineEdit[GUI_LINE_EDIT_SEND_ID]);
  SetLineEditPreText(&s_arrLineEdit[GUI_LINE_EDIT_SEND_ID], "ID:");

  //发送数据
  InitLineEditWidgetStruct(&s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA]);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].x0 = 255;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].y0 = 405;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].width = 200;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].height = 65;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].textX0 = 3;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].textY0 = 19;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].hasFocusBackgroundImage = (void*)(&s_structHasFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].noFocusBackgroundImage = (void*)(&s_structNoFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].pressCallback = LineEditPressCallback;
  CreateLineEditWidget(&s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA]);
  SetLineEditPreText(&s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA], "DATA:");

  //终端控件
  InitTerminalWidgetStruct(&s_structTerminal);
  s_structTerminalImage.image = (unsigned char*)CAN_TERMINAL_BACKGROUND_DIR;
  s_structTerminal.x0 = 25;
  s_structTerminal.y0 = 55;
  s_structTerminal.width = 430;
  s_structTerminal.height = 340;
  s_structTerminal.topOff = 30;
  s_structTerminal.showSpace = 3;
  s_structTerminal.textColor = GUI_COLOR_WHITE;
  s_structTerminal.textFont = GUI_FONT_ASCII_24;
  s_structTerminal.textHeight = 30;
  s_structTerminal.background = NULL;
  s_structTerminal.backImage = (void*)(&s_structTerminalImage);
  s_structTerminal.imageType = GUI_IMAGE_TYPE_JPEG;
  CreateTerminalWidget(&s_structTerminal);
  TerminalWidgetClear(&s_structTerminal);

  //CONX显示
  LCDShowString(545, 58, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "CAN0");

  //速度显示
  LCDShowString(545, 83, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "1MBps");

  //标识符显示
  LCDShowString(690, 58, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "0x5A5");

  //掩码显示
  LCDShowString(690, 83, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "0xFFFFFFFF");

  //初始化CAN模块
  InitCAN();
}

/*********************************************************************************************************
* 函数名称：DeleteGUICAN
* 函数功能：注销GUI CAN界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUICAN(void)
{
  //注销CAN模块
  DeInitCAN();
}

/*********************************************************************************************************
* 函数名称：GUICANPoll
* 函数功能：GUI CAN轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUICANPoll(void)
{
  const char s_arrButtonValue[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  EnumGUIButtons button;
  EnumGUILineEdit lineEdit;
  char* data = NULL;
  u32   id   = 0;

  //行编辑模块扫描
  ScanGUILineEdit();

  //按键扫描
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //发送处理
    if(GUI_BUTTON_SEND == button)
    {
      if((0 == GetGUILineEditDataResult(&data)) && (0 == GetGUILineEditIDResult(&id)))
      {
        SendProc(id, data);
      }
    }

    //删除处理
    else if(GUI_BUTTON_BACK == button)
    {
      for(lineEdit = (EnumGUILineEdit)0; lineEdit < GUI_LINE_EDIT_MAX; lineEdit++)
      {
        if(1 == s_arrLineEdit[lineEdit].hasFocus)
        {
          DeleteCodeFromLineEdit(&s_arrLineEdit[lineEdit]);
        }
      }
    }

    //K0-KF按下
    else
    {
      for(lineEdit = (EnumGUILineEdit)0; lineEdit < GUI_LINE_EDIT_MAX; lineEdit++)
      {
        if(1 == s_arrLineEdit[lineEdit].hasFocus)
        {
          AddrCharToLineEdit(&s_arrLineEdit[lineEdit], s_arrButtonValue[button]);
        }
      }
    }
  }

  //接收数据并更新到终端显示
  ReadProc();
}
