/*********************************************************************************************************
* 模块名称：GUIRS232.c
* 摘    要：GUI RS232通信模块
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
#include "GUIRS232.h"
#include "GUIPlatform.h"
#include "ButtonWidget.h"
#include "LineEditWidget.h"
#include "TerminalWidget.h"
#include "BMP.h"
#include "JPEG.h"
#include "Common.h"
#include "TLILCD.h"
#include "UART5.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define TEXT_LEN_MAX 64 //显示字符最大长度

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
typedef enum
{
  GUI_BUTTON_K7 = 0, //K7按键，必须是从0开始
  GUI_BUTTON_K8,     //K8按键
  GUI_BUTTON_K9,     //K9按键
  GUI_BUTTON_K4,     //K4按键
  GUI_BUTTON_K5,     //K5按键
  GUI_BUTTON_K6,     //K6按键
  GUI_BUTTON_K1,     //K1按键
  GUI_BUTTON_K2,     //K2按键
  GUI_BUTTON_K3,     //K3按键
  GUI_BUTTON_SEND,   //发送按钮
  GUI_BUTTON_K0,     //K0按键
  GUI_BUTTON_BACK,   //删除按钮
  GUI_BUTTON_MAX,    //按键数量
  GUI_BUTTON_NONE,   //无效按键
}EnumGUIButtons;

typedef enum
{
  GUI_LINE_EDIT_SEND_DATA = 0, //发送数据，必须是从0开始
  GUI_LINE_EDIT_MAX,           //控件数量
  GUI_LINE_EDIT_NONE,          //无效控件
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
static void ButtonPressCallback(void* button);     //按键回调函数
static EnumGUIButtons ScanGUIButton(void);         //按键扫描
static void LineEditPressCallback(void* lineEdit); //行编辑回调函数
static void ScanGUILineEdit(void);                 //行编辑模块扫描
static u8   GetGUILineEditRusult(EnumGUILineEdit lineEdit, char** result); //获取行编辑控件输入结果
static void SendProc(char* string); //发送处理函数
static void ReadProc(void);         //读串口处理

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
* 注    意：
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
* 函数名称：GetGUILineEditRusult
* 函数功能：获取行编辑控件输入结果
* 输入参数：lineEdit：控件枚举，result：用于返回输入结果
* 输出参数：void
* 返 回 值：0-获取成功，其它：获取失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u8 GetGUILineEditRusult(EnumGUILineEdit lineEdit, char** result)
{
  //控件不再许可范围
  if(lineEdit >= GUI_LINE_EDIT_MAX)
  {
    result = NULL;
    return 1;
  }

  //当前行编辑中有数据并且处于获得焦点状态
  if((EDIT_LINE_HAS_FOCUS == s_arrLineEdit[lineEdit].hasFocus) && (0 != s_arrLineEdit[lineEdit].text[0]))
  {
    *result = s_arrLineEdit[lineEdit].text;
    return 0;
  }
  
  //无数据或并未获得焦点
  else
  {
    result = NULL;
    return 1;
  }
}

/*********************************************************************************************************
* 函数名称：SendProc
* 函数功能：发送处理函数
* 输入参数：string：行输入结果
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void SendProc(char* string)
{
  u8 i;        //发送计数
  u8 sendData; //发送数据

  //校验输入地址是否为空指针
  if(NULL == string)
  {
    return;
  }

  //通过UART5将数据发送出去
  i = 0;
  while(0 != *(string + i))
  {
    //获取需要发送的数据
    sendData = *(string + i);

    //通过串口发送出去
    WriteUART5(&sendData, 1);

    //发送计数加一
    i++;
  }

  //发送回车换行
  sendData = '\r';
  WriteUART5(&sendData, 1);
  sendData = '\n';
  WriteUART5(&sendData, 1);
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
  static u8  s_arrReadBuf[48] = {0}; //接收缓冲区
  static u8  s_iReadCnt       = 0;   //接收计数
  static u64 s_iBeginTime     = 0;   //接收到第一个字符时的系统时间

  u8  readData;
  u64 time;

  //获取系统运行时间
  time = GUIGetSysTime();

  //接收数据处理
  while(ReadUART5(&readData, 1))
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
        TerminalWidgetShowLine(&s_structTerminal, (char*)s_arrReadBuf);
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
    TerminalWidgetShowLine(&s_structTerminal, (char*)s_arrReadBuf);
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUIRS232
* 函数功能：创建GUI RS232界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIRS232(void)
{
  //按键横坐标
  static const u16 s_arrButtonX[GUI_BUTTON_MAX] = {560, 640, 720,  //K7-K9
                                                   560, 640, 720,  //K4-K5
                                                   560, 640, 720,  //K1-K3
                                                   560, 640, 720}; //SEND-BACK

  //按键纵坐标
  static const u16 s_arrButtonY[GUI_BUTTON_MAX] = {220, 220, 220,  //K7-K9
                                                   285, 285, 285,  //K4-K5
                                                   350, 350, 350,  //K1-K3
                                                   415, 415, 415}; //SEND-BACK

  //按键显示字符
  static const char* s_arrButtonText[GUI_BUTTON_MAX] = {"7","8", "9", "4", "5", "6", "1", "2", "3", "Send", "0", "Back"};
  
  //按键按下、抬起图片
  static StructBmpImage s_structKeyPressImage;
  static StructBmpImage s_structKeyReleaseImage;

  //行输入相关图片
  static StructJpegImage s_structHasFocusImage;
  static StructJpegImage s_structNoFocusImage;

  //终端相关图片
  static StructJpegImage s_structTerminalImage;

  u32 i;

  //LCD横屏显示
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //显示背景图片
  DisPlayBMPByIPA(0, 0, RS232_BACKGROUND_DIR);

  //按键按下背景图片
  s_structKeyPressImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyPressImage.alphaGate   = 200;
  s_structKeyPressImage.storageType = BMP_IN_FATFS;
  s_structKeyPressImage.addr        = (void*)RS232_BUTTON_PRESS_DIR;

  //按键释放背景图片
  s_structKeyReleaseImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyReleaseImage.alphaGate   = 200;
  s_structKeyReleaseImage.storageType = BMP_IN_FATFS;
  s_structKeyReleaseImage.addr        = (void*)RS232_BUTTON_RELEASE_DIR;

  //标记无按键按下
  s_enumButtonPress = GUI_BUTTON_NONE;

  //创建K0-KF
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    InitButtonWidgetStruct(&s_arrButton[i]);
    s_arrButton[i].x0 = s_arrButtonX[i];
    s_arrButton[i].y0 = s_arrButtonY[i];
    s_arrButton[i].width = 60;
    s_arrButton[i].height = 45;

    if((GUI_BUTTON_SEND == i) || (GUI_BUTTON_BACK == i))
    {
      s_arrButton[i].textX0 = 6;
    }
    else
    {
      s_arrButton[i].textX0 = 23;
    }
    
    s_arrButton[i].textY0 = 10;
    s_arrButton[i].text = s_arrButtonText[i];
    s_arrButton[i].pressBackgroudImage = (void*)(&s_structKeyPressImage);
    s_arrButton[i].releaseBackgroudImage = (void*)(&s_structKeyReleaseImage);
    s_arrButton[i].pressCallback = ButtonPressCallback;
    CreateButtonWidget(&s_arrButton[i]);
  }

  //行编辑控件
  InitLineEditWidgetStruct(&s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA]);
  s_structHasFocusImage.image = (unsigned char*)RS232_LINE_EDIT_HAS_FOCUS_DIR;
  s_structNoFocusImage.image = (unsigned char*)RS232_LINE_EDIT_NO_FOCUS_DIR;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].x0 = 560;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].y0 = 145;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].width = 220;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].height = 50;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].textX0 = 3;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].textY0 = 10;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].hasFocusBackgroundImage = (void*)(&s_structHasFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].noFocusBackgroundImage = (void*)(&s_structNoFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].pressCallback = LineEditPressCallback;
  CreateLineEditWidget(&s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA]);

  //终端控件
  InitTerminalWidgetStruct(&s_structTerminal);
  s_structTerminalImage.image = (unsigned char*)RS232_TERMINAL_DIR;
  s_structTerminal.x0 = 20;
  s_structTerminal.y0 = 60;
  s_structTerminal.width = 520;
  s_structTerminal.height = 400;
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

  //显示串口号
  LCDShowString(620, 60, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "UART5");

  //显示波特率
  LCDShowString(620, 80, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "115200");

  //配置UART5
  InitUART5(115200);
}

/*********************************************************************************************************
* 函数名称：DeleteGUIRS232
* 函数功能：注销GUI RS232界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUIRS232(void)
{
  DeInitUART5();
}

/*********************************************************************************************************
* 函数名称：GUIRS232Poll
* 函数功能：GUI RS232轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIRS232Poll(void)
{
  const char      s_arrButtonValue[12] = {'7', '8', '9', '4', '5', '6', '1', '2', '3', 'S', '0', 'B'};
  EnumGUIButtons  button;
  EnumGUILineEdit lineEdit;
  char*           string = NULL;

  //按键扫描
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //发送处理
    if(GUI_BUTTON_SEND == button)
    {
      if(0 == GetGUILineEditRusult(GUI_LINE_EDIT_SEND_DATA, &string))
      {
        SendProc(string);
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

    //K0-K9按下
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
       
  //行编辑模块扫描
  ScanGUILineEdit();

  //接收数据处理
  ReadProc();
}
