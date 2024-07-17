/*********************************************************************************************************
* 模块名称：GUIEthernet.c
* 摘    要：GUI 以太网通信模块
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
#include "GUIEthernet.h"
#include "GUIPlatform.h"
#include "ButtonWidget.h"
#include "LineEditWidget.h"
#include "TerminalWidget.h"
#include "TLILCD.h"
#include "BMP.h"
#include "JPEG.h"
#include "Common.h"
#include "SysTick.h"
#include "Timer.h"
#include "stdio.h"
#include "LWIPComm.h"
#include "TCPServerBase.h"
#include "RTC.h"
#include "Malloc.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define MAX_STRING_LEN 64   //显示字符最大长度

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
  GUI_BUTTON_POINT,  //小点按键
  GUI_BUTTON_K0,     //K0按键
  GUI_BUTTON_BACK,   //删除按钮
  GUI_BUTTON_SEND,   //发送按钮
  GUI_BUTTON_MAX,    //按键数量
  GUI_BUTTON_NONE,   //无效按键
}EnumGUIButtons;

typedef enum
{
  GUI_LINE_EDIT_CLIENT_IP = 0, //客户端IP，必须是从0开始
  GUI_LINE_EDIT_SEND_DATA,     //发送数据
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

//字符串转换缓冲区
static char s_arrStringBuff[MAX_STRING_LEN]; 

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void ButtonPressCallback(void* button);      //按键回调函数
static EnumGUIButtons ScanGUIButton(void);          //按键扫描
static void LineEditPressCallback(void* lineEdit);  //行编辑回调函数
static void ScanGUILineEdit(void);                  //行编辑模块扫描
static void ReadClientData(void);                   //读取客户端数据并打印输出到终端
static void SendDataToClient(char* ip, char* data); //发送数据给指定客户端

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
* 函数名称：ReadClientData
* 函数功能：读取客户端数据并打印输出到终端
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ReadClientData(void)
{
  static char s_arrReadBuf[64]; //读取缓冲区
  u32 readCnt;                  //读取到的数据量
  u32 i;                        //循环变量
  u32 ipAddr;                   //客户端IP地址
  u8  ip[4];                    //客户端IP地址拆解
  u32 hour, minute, second;     //系统运行时间（小时、分钟、秒）

  //循环检查每个客户端是否有发送数据到服务器
  for(i = 0; i < TCP_SERVER_CLIENT_NUM; i++)
  {
    //获取客户端IP地址
    ipAddr = GetClientIP((EnumTCPClientList)i);

    //一次性读出客户端所有数据数据，最大读取量为缓冲区长度-1
    readCnt = ReadTCPServer(i, (u8*)s_arrReadBuf, (sizeof(s_arrReadBuf) / sizeof(u8)) - 1);

    //IP地址不为零，表示有客户端接上，readCnt大于零则表示成功读取到数据
    if(0 != ipAddr && readCnt > 0)
    {
      //获取实时时钟时间
      RTCGetTime(&hour, &minute, &second);

      //标记字符串结尾
      s_arrReadBuf[readCnt] = 0;

      //终端输出一个空行
      TerminalWidgetShowLine(&s_structTerminal, "");

      //打印客户端IP地址到终端
      ip[0] = (ipAddr >> 0 ) & 0xff; //IADDR4
      ip[1] = (ipAddr >> 8 ) & 0xff; //IADDR3
      ip[2] = (ipAddr >> 16) & 0xff; //IADDR2
      ip[3] = (ipAddr >> 24) & 0xff; //IADDR1
      sprintf(s_arrStringBuff, "<receive ip: %d.%d.%d.%d  %02d:%02d:%02d>", ip[0], ip[1], ip[2], ip[3], hour, minute, second);
      TerminalWidgetShowLine(&s_structTerminal, s_arrStringBuff);

      //打印客户端数据到终端
      TerminalWidgetShowLine(&s_structTerminal, s_arrReadBuf);
    }
  }
}

/*********************************************************************************************************
* 函数名称：SendDataToClient
* 函数功能：发送数据给指定客户端
* 输入参数：ip：客户端IP地址，data：要发送的数据
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void SendDataToClient(char* ip, char* data)
{
  u8  sum;      //和，解析IP地址时使用
  u8  i;        //循环变量，解析IP地址时使用
  u8  strCnt;   //字符计数，解析IP地址时使用
  u8  ipBuf[4]; //IP地址（拆分形式）
  u32 ipAddr;   //IP地址（合并形式）
  u32 sendNum;  //需要发送的数据量
  u32 ret;      //发送数据返回值，表示成功发送的数据量
  u32 hour, minute, second; //系统运行时间（小时、分钟、秒）

  //获取实时时钟时间
  RTCGetTime(&hour, &minute, &second);

  //解析出IP地址
  strCnt = 0;
  for(i = 0; i < 4; i++)
  {
    sum = 0;
    while((ip[strCnt] >= '0') && (ip[strCnt] <= '9'))
    {
      sum = sum * 10 + (ip[strCnt] - 48);
      strCnt++;
    }
    strCnt++;

    ipBuf[i] = sum;
  }
  ipAddr = (ipBuf[0] << 0) | (ipBuf[1] << 8) | (ipBuf[2] << 16) | (ipBuf[3] << 24);

  //统计发送数据量
  sendNum = 0;
  while(0 != data[sendNum])
  {
    sendNum++;
  }

  //将数据通过TCP服务器发送至客户端
  ret = WriteTCPServer(ipAddr, (u8*)data, sendNum);

  //输出一个空行
  TerminalWidgetShowLine(&s_structTerminal, "");

  //输出时间信息
  sprintf(s_arrStringBuff, "<send ip: %d.%d.%d.%d  %02d:%02d:%02d>", ipBuf[0], ipBuf[1], ipBuf[2], ipBuf[3], hour, minute, second);
  TerminalWidgetShowLine(&s_structTerminal, (char*)s_arrStringBuff);

  if(0 != ret)
  {
    //发送成功
    TerminalWidgetShowLine(&s_structTerminal, (char*)data);
  }
  else
  {
    //发送失败
    TerminalWidgetShowLine(&s_structTerminal, (char*)"Failed to send");
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUIEthernet
* 函数功能：创建GUI Ethernet界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIEthernet(void)
{
  static const u16 s_arrButtonX[GUI_BUTTON_MAX] = {560, 640, 720,  //K7-K9
                                                   560, 640, 720,  //K4-K5
                                                   560, 640, 720,  //K1-K3
                                                   560, 640, 720}; //POINT-BACK

  static const u16 s_arrButtonY[GUI_BUTTON_MAX] = {213, 213, 213,  //K7-K9
                                                   263, 263, 263,  //K4-K5
                                                   313, 313, 313,  //K1-K3
                                                   363, 363, 363}; //POINT-BACK

  static const char* s_arrButtonText[16] = {"7","8", "9", "4", "5", "6", "1", "2", "3", ".", "0", "<-"};
  static StructBmpImage s_structKeyNumPressImage;
  static StructBmpImage s_structKeyNumReleaseImage;
  static StructBmpImage s_structKeySendPressImage;
  static StructBmpImage s_structKeySendReleaseImage;
  static StructJpegImage s_structHasFocusImage;
  static StructJpegImage s_structNoFocusImage;
  static StructJpegImage s_structTerminalImage;
  EnumGUIButtons i;
  StructLWIPDev* lwipDev;

  //LCD横屏显示
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //显示背景图片
  DisPlayBMPByIPA(0, 0, ETHERNET_BACKGROUND_DIR);

  //初始化按键背景图片
  s_structKeyNumPressImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyNumPressImage.alphaGate   = 200;
  s_structKeyNumPressImage.storageType = BMP_IN_FATFS;
  s_structKeyNumPressImage.addr        = (void*)ETHERNET_BUTTON_PRESS_60x45_DIR;

  s_structKeyNumReleaseImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyNumReleaseImage.alphaGate   = 200;
  s_structKeyNumReleaseImage.storageType = BMP_IN_FATFS;
  s_structKeyNumReleaseImage.addr        = (void*)ETHERNET_BUTTON_RELEASE_60x45_DIR;

  s_structKeySendPressImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeySendPressImage.alphaGate   = 200;
  s_structKeySendPressImage.storageType = BMP_IN_FATFS;
  s_structKeySendPressImage.addr        = (void*)ETHERNET_BUTTON_PRESS_220x45_DIR;

  s_structKeySendReleaseImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeySendReleaseImage.alphaGate   = 200;
  s_structKeySendReleaseImage.storageType = BMP_IN_FATFS;
  s_structKeySendReleaseImage.addr        = (void*)ETHERNET_BUTTON_RELEASE_220x45_DIR;

  //标记无按键按下
  s_enumButtonPress = GUI_BUTTON_NONE;

  //创建K0-KF
  for(i = (EnumGUIButtons)0; i <= GUI_BUTTON_BACK; i++)
  {
    InitButtonWidgetStruct(&s_arrButton[i]);
    s_arrButton[i].x0 = s_arrButtonX[i];
    s_arrButton[i].y0 = s_arrButtonY[i];
    s_arrButton[i].width = 60;
    s_arrButton[i].height = 45;

    if(GUI_BUTTON_BACK == i)
    {
      s_arrButton[i].textX0 = 16;
      s_arrButton[i].textY0 = 10;
    }
    else if(GUI_BUTTON_POINT == i)
    {
      s_arrButton[i].textX0 = 25;
      s_arrButton[i].textY0 = 4;
    }
    else
    {
      s_arrButton[i].textX0 = 23;
      s_arrButton[i].textY0 = 10;
    }
    
    
    s_arrButton[i].text = s_arrButtonText[i];
    s_arrButton[i].pressBackgroudImage = (void*)(&s_structKeyNumPressImage);
    s_arrButton[i].releaseBackgroudImage = (void*)(&s_structKeyNumReleaseImage);
    s_arrButton[i].pressCallback = ButtonPressCallback;
    CreateButtonWidget(&s_arrButton[i]);
  }

  //创建发送按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_SEND]);
  s_arrButton[GUI_BUTTON_SEND].x0 = 560;
  s_arrButton[GUI_BUTTON_SEND].y0 = 413;
  s_arrButton[GUI_BUTTON_SEND].width = 220;
  s_arrButton[GUI_BUTTON_SEND].height = 45;
  s_arrButton[GUI_BUTTON_SEND].textX0 = 86;
  s_arrButton[GUI_BUTTON_SEND].textY0 = 10;
  s_arrButton[GUI_BUTTON_SEND].text = "SEND";
  s_arrButton[GUI_BUTTON_SEND].pressBackgroudImage = (void*)(&s_structKeySendPressImage);
  s_arrButton[GUI_BUTTON_SEND].releaseBackgroudImage = (void*)(&s_structKeySendReleaseImage);
  s_arrButton[GUI_BUTTON_SEND].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_SEND]);

  //初始化行编辑背景图片
  s_structHasFocusImage.image = (unsigned char*)ETHERNET_LINE_EDIT_HAS_FOCUS_220x45_DIR;
  s_structNoFocusImage.image = (unsigned char*)ETHERNET_LINE_EDIT_NO_FOCUS_220x45_DIR;

  //发送ID
  InitLineEditWidgetStruct(&s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP]);
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].x0 = 560;
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].y0 = 105;
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].width = 220;
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].height = 45;
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].textX0 = 2;
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].textY0 = 10;
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].hasFocusBackgroundImage = (void*)(&s_structHasFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].noFocusBackgroundImage = (void*)(&s_structNoFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].pressCallback = LineEditPressCallback;
  CreateLineEditWidget(&s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP]);
  SetLineEditPreText(&s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP], "IP:");

  //发送数据
  InitLineEditWidgetStruct(&s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA]);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].x0 = 560;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].y0 = 160;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].width = 220;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].height = 45;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].textX0 = 3;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].textY0 = 10;
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].hasFocusBackgroundImage = (void*)(&s_structHasFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].noFocusBackgroundImage = (void*)(&s_structNoFocusImage);
  s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].pressCallback = LineEditPressCallback;
  CreateLineEditWidget(&s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA]);
  SetLineEditPreText(&s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA], "DATA:");

  //终端控件
  InitTerminalWidgetStruct(&s_structTerminal);
  s_structTerminalImage.image = (unsigned char*)ETHERNET_TERMINAL_BACKGROUND_DIR;
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

  //获取LWIP设备结构体地址，用于打印TCP服务器IP地址
  lwipDev = GetLWIPCommDev();

  //LWIP初始化
  TerminalWidgetShowLine(&s_structTerminal, "Initializing LWIP");
  while(LWIPCommInit())
  {
    TerminalWidgetShowLine(&s_structTerminal, "LWIP Init Falied!");
    DelayNms(1200);
    TerminalWidgetShowLine(&s_structTerminal, "Retrying...");
  }
  TerminalWidgetShowLine(&s_structTerminal, "LWIP Init Success!");
  DelayNms(500);

#if LWIP_DHCP   //使用DHCP（自动获取本机IP地址）
  //等待DHCP获取成功/超时溢出
  TerminalWidgetShowLine(&s_structTerminal, "DHCP IP configing...");
  while((lwipDev->dhcpstatus!=2)&&(lwipDev->dhcpstatus!=0XFF))
  {
    LWIPPeriodicHandle();
    LWIPReadPackHandle();
  }
  TerminalWidgetShowLine(&s_structTerminal, "DHCP IP configing finish");
#endif

  //创建TCP服务器
  CreateTCPServer();

  //显示服务器IP
  sprintf(s_arrStringBuff, "Server IP: %d.%d.%d.%d", lwipDev->ip[0], lwipDev->ip[1], lwipDev->ip[2], lwipDev->ip[3]);
  GUIDrawTextLine(560, 56, (u32)s_arrStringBuff, GUI_FONT_ASCII_16, NULL, GUI_COLOR_WHITE, 1, 100);

  //显示服务器端口
  sprintf(s_arrStringBuff, "Server Port: %d", TCP_SERVER_PORT);
  GUIDrawTextLine(560, 78, (u32)s_arrStringBuff, GUI_FONT_ASCII_16, NULL, GUI_COLOR_WHITE, 1, 100);
}

/*********************************************************************************************************
* 函数名称：DeleteGUIEthernet
* 函数功能：注销GUI Ethernet界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUIEthernet(void)
{
  CloseTCPServer(); //关闭TCP服务器
  DeInitLWIPComm(); //注销LWIP，复位以太网底层驱动
}

/*********************************************************************************************************
* 函数名称：GUIEthernetPoll
* 函数功能：GUI Ethernet轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIEthernetPoll(void)
{
  const char s_arrButtonValue[] = {'7', '8', '9', '4', '5', '6', '1', '2', '3', '.', '0'};
  EnumGUIButtons button;
  EnumGUILineEdit lineEdit;
  char* ip;
  char* data;

  //按键扫描
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //K0-KF，行编辑更新显示
    if(button <= GUI_BUTTON_K0)
    {
      for(lineEdit = (EnumGUILineEdit)0; lineEdit < GUI_LINE_EDIT_MAX; lineEdit++)
      {
        if(1 == s_arrLineEdit[lineEdit].hasFocus)
        {
          AddrCharToLineEdit(&s_arrLineEdit[lineEdit], s_arrButtonValue[button]);
        }
      }
    }

    //删除键
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

    //发送键
    else if(GUI_BUTTON_SEND)
    {
      if((0 != s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].text[0]) && (0 != s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].text[0]))
      {
        ip = s_arrLineEdit[GUI_LINE_EDIT_CLIENT_IP].text;
        data = s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].text;
        SendDataToClient(ip, data);
      }
    }
  }

  //行编辑模块扫描
  ScanGUILineEdit();

  //LWIP轮询任务
  LWIPPeriodicHandle();

  //LWIP接收数据包轮询任务
  LWIPReadPackHandle();

  //接收客户端数据并输出打印到终端
  ReadClientData();
}
