/*********************************************************************************************************
* ģ�����ƣ�GUIEthernet.c
* ժ    Ҫ��GUI ��̫��ͨ��ģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* ������ڣ�2021��07��01��
* ��    �ݣ�
* ע    �⣺
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
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
*                                              �궨��
*********************************************************************************************************/
#define MAX_STRING_LEN 64   //��ʾ�ַ���󳤶�

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
typedef enum
{
  GUI_BUTTON_K7 = 0, //K7�����������Ǵ�0��ʼ
  GUI_BUTTON_K8,     //K8����
  GUI_BUTTON_K9,     //K9����
  GUI_BUTTON_K4,     //K4����
  GUI_BUTTON_K5,     //K5����
  GUI_BUTTON_K6,     //K6����
  GUI_BUTTON_K1,     //K1����
  GUI_BUTTON_K2,     //K2����
  GUI_BUTTON_K3,     //K3����
  GUI_BUTTON_POINT,  //С�㰴��
  GUI_BUTTON_K0,     //K0����
  GUI_BUTTON_BACK,   //ɾ����ť
  GUI_BUTTON_SEND,   //���Ͱ�ť
  GUI_BUTTON_MAX,    //��������
  GUI_BUTTON_NONE,   //��Ч����
}EnumGUIButtons;

typedef enum
{
  GUI_LINE_EDIT_CLIENT_IP = 0, //�ͻ���IP�������Ǵ�0��ʼ
  GUI_LINE_EDIT_SEND_DATA,     //��������
  GUI_LINE_EDIT_MAX,           //�ؼ�����
  GUI_LINE_EDIT_NONE,          //��Ч�ؼ�
}EnumGUILineEdit;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
//�������
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX]; //��������
static EnumGUIButtons     s_enumButtonPress;           //��������Ǹ���������

//�б༭���
static StructLineEditWidget s_arrLineEdit[GUI_LINE_EDIT_MAX];

//�ն����
static StructTerminalWidget s_structTerminal;

//�ַ���ת��������
static char s_arrStringBuff[MAX_STRING_LEN]; 

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ButtonPressCallback(void* button);      //�����ص�����
static EnumGUIButtons ScanGUIButton(void);          //����ɨ��
static void LineEditPressCallback(void* lineEdit);  //�б༭�ص�����
static void ScanGUILineEdit(void);                  //�б༭ģ��ɨ��
static void ReadClientData(void);                   //��ȡ�ͻ������ݲ���ӡ������ն�
static void SendDataToClient(char* ip, char* data); //�������ݸ�ָ���ͻ���

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ButtonPressCallback
* �������ܣ������ص�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�ScanGUIButton
* �������ܣ�����ɨ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺ÿ��20����ִ��һ��
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
* �������ƣ�LineEditPressCallback
* �������ܣ��б༭�ص�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺���ҽ���һ���б༭�ؼ��ܻ�ȡ����
*********************************************************************************************************/
static void LineEditPressCallback(void* lineEdit)
{
  u8 i;
  for(i = 0; i < GUI_LINE_EDIT_MAX; i++)
  {
    if((StructLineEditWidget*)lineEdit != &s_arrLineEdit[i])
    {
      //ȡ���ؼ�����״̬
      ClearLineEditWidgetFocus(&s_arrLineEdit[i]);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�ScanGUILineEdit
* �������ܣ��б༭ģ��ɨ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�ReadClientData
* �������ܣ���ȡ�ͻ������ݲ���ӡ������ն�
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ReadClientData(void)
{
  static char s_arrReadBuf[64]; //��ȡ������
  u32 readCnt;                  //��ȡ����������
  u32 i;                        //ѭ������
  u32 ipAddr;                   //�ͻ���IP��ַ
  u8  ip[4];                    //�ͻ���IP��ַ���
  u32 hour, minute, second;     //ϵͳ����ʱ�䣨Сʱ�����ӡ��룩

  //ѭ�����ÿ���ͻ����Ƿ��з������ݵ�������
  for(i = 0; i < TCP_SERVER_CLIENT_NUM; i++)
  {
    //��ȡ�ͻ���IP��ַ
    ipAddr = GetClientIP((EnumTCPClientList)i);

    //һ���Զ����ͻ��������������ݣ�����ȡ��Ϊ����������-1
    readCnt = ReadTCPServer(i, (u8*)s_arrReadBuf, (sizeof(s_arrReadBuf) / sizeof(u8)) - 1);

    //IP��ַ��Ϊ�㣬��ʾ�пͻ��˽��ϣ�readCnt���������ʾ�ɹ���ȡ������
    if(0 != ipAddr && readCnt > 0)
    {
      //��ȡʵʱʱ��ʱ��
      RTCGetTime(&hour, &minute, &second);

      //����ַ�����β
      s_arrReadBuf[readCnt] = 0;

      //�ն����һ������
      TerminalWidgetShowLine(&s_structTerminal, "");

      //��ӡ�ͻ���IP��ַ���ն�
      ip[0] = (ipAddr >> 0 ) & 0xff; //IADDR4
      ip[1] = (ipAddr >> 8 ) & 0xff; //IADDR3
      ip[2] = (ipAddr >> 16) & 0xff; //IADDR2
      ip[3] = (ipAddr >> 24) & 0xff; //IADDR1
      sprintf(s_arrStringBuff, "<receive ip: %d.%d.%d.%d  %02d:%02d:%02d>", ip[0], ip[1], ip[2], ip[3], hour, minute, second);
      TerminalWidgetShowLine(&s_structTerminal, s_arrStringBuff);

      //��ӡ�ͻ������ݵ��ն�
      TerminalWidgetShowLine(&s_structTerminal, s_arrReadBuf);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�SendDataToClient
* �������ܣ��������ݸ�ָ���ͻ���
* ���������ip���ͻ���IP��ַ��data��Ҫ���͵�����
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void SendDataToClient(char* ip, char* data)
{
  u8  sum;      //�ͣ�����IP��ַʱʹ��
  u8  i;        //ѭ������������IP��ַʱʹ��
  u8  strCnt;   //�ַ�����������IP��ַʱʹ��
  u8  ipBuf[4]; //IP��ַ�������ʽ��
  u32 ipAddr;   //IP��ַ���ϲ���ʽ��
  u32 sendNum;  //��Ҫ���͵�������
  u32 ret;      //�������ݷ���ֵ����ʾ�ɹ����͵�������
  u32 hour, minute, second; //ϵͳ����ʱ�䣨Сʱ�����ӡ��룩

  //��ȡʵʱʱ��ʱ��
  RTCGetTime(&hour, &minute, &second);

  //������IP��ַ
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

  //ͳ�Ʒ���������
  sendNum = 0;
  while(0 != data[sendNum])
  {
    sendNum++;
  }

  //������ͨ��TCP�������������ͻ���
  ret = WriteTCPServer(ipAddr, (u8*)data, sendNum);

  //���һ������
  TerminalWidgetShowLine(&s_structTerminal, "");

  //���ʱ����Ϣ
  sprintf(s_arrStringBuff, "<send ip: %d.%d.%d.%d  %02d:%02d:%02d>", ipBuf[0], ipBuf[1], ipBuf[2], ipBuf[3], hour, minute, second);
  TerminalWidgetShowLine(&s_structTerminal, (char*)s_arrStringBuff);

  if(0 != ret)
  {
    //���ͳɹ�
    TerminalWidgetShowLine(&s_structTerminal, (char*)data);
  }
  else
  {
    //����ʧ��
    TerminalWidgetShowLine(&s_structTerminal, (char*)"Failed to send");
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUIEthernet
* �������ܣ�����GUI Ethernet����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //��ʾ����ͼƬ
  DisPlayBMPByIPA(0, 0, ETHERNET_BACKGROUND_DIR);

  //��ʼ����������ͼƬ
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

  //����ް�������
  s_enumButtonPress = GUI_BUTTON_NONE;

  //����K0-KF
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

  //�������Ͱ�ť
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

  //��ʼ���б༭����ͼƬ
  s_structHasFocusImage.image = (unsigned char*)ETHERNET_LINE_EDIT_HAS_FOCUS_220x45_DIR;
  s_structNoFocusImage.image = (unsigned char*)ETHERNET_LINE_EDIT_NO_FOCUS_220x45_DIR;

  //����ID
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

  //��������
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

  //�ն˿ؼ�
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

  //��ȡLWIP�豸�ṹ���ַ�����ڴ�ӡTCP������IP��ַ
  lwipDev = GetLWIPCommDev();

  //LWIP��ʼ��
  TerminalWidgetShowLine(&s_structTerminal, "Initializing LWIP");
  while(LWIPCommInit())
  {
    TerminalWidgetShowLine(&s_structTerminal, "LWIP Init Falied!");
    DelayNms(1200);
    TerminalWidgetShowLine(&s_structTerminal, "Retrying...");
  }
  TerminalWidgetShowLine(&s_structTerminal, "LWIP Init Success!");
  DelayNms(500);

#if LWIP_DHCP   //ʹ��DHCP���Զ���ȡ����IP��ַ��
  //�ȴ�DHCP��ȡ�ɹ�/��ʱ���
  TerminalWidgetShowLine(&s_structTerminal, "DHCP IP configing...");
  while((lwipDev->dhcpstatus!=2)&&(lwipDev->dhcpstatus!=0XFF))
  {
    LWIPPeriodicHandle();
    LWIPReadPackHandle();
  }
  TerminalWidgetShowLine(&s_structTerminal, "DHCP IP configing finish");
#endif

  //����TCP������
  CreateTCPServer();

  //��ʾ������IP
  sprintf(s_arrStringBuff, "Server IP: %d.%d.%d.%d", lwipDev->ip[0], lwipDev->ip[1], lwipDev->ip[2], lwipDev->ip[3]);
  GUIDrawTextLine(560, 56, (u32)s_arrStringBuff, GUI_FONT_ASCII_16, NULL, GUI_COLOR_WHITE, 1, 100);

  //��ʾ�������˿�
  sprintf(s_arrStringBuff, "Server Port: %d", TCP_SERVER_PORT);
  GUIDrawTextLine(560, 78, (u32)s_arrStringBuff, GUI_FONT_ASCII_16, NULL, GUI_COLOR_WHITE, 1, 100);
}

/*********************************************************************************************************
* �������ƣ�DeleteGUIEthernet
* �������ܣ�ע��GUI Ethernet����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUIEthernet(void)
{
  CloseTCPServer(); //�ر�TCP������
  DeInitLWIPComm(); //ע��LWIP����λ��̫���ײ�����
}

/*********************************************************************************************************
* �������ƣ�GUIEthernetPoll
* �������ܣ�GUI Ethernet��ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIEthernetPoll(void)
{
  const char s_arrButtonValue[] = {'7', '8', '9', '4', '5', '6', '1', '2', '3', '.', '0'};
  EnumGUIButtons button;
  EnumGUILineEdit lineEdit;
  char* ip;
  char* data;

  //����ɨ��
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //K0-KF���б༭������ʾ
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

    //ɾ����
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

    //���ͼ�
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

  //�б༭ģ��ɨ��
  ScanGUILineEdit();

  //LWIP��ѯ����
  LWIPPeriodicHandle();

  //LWIP�������ݰ���ѯ����
  LWIPReadPackHandle();

  //���տͻ������ݲ������ӡ���ն�
  ReadClientData();
}
