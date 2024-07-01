/*********************************************************************************************************
* ģ�����ƣ�GUIRS232.c
* ժ    Ҫ��GUI RS232ͨ��ģ��
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
*                                              �궨��
*********************************************************************************************************/
#define TEXT_LEN_MAX 64 //��ʾ�ַ���󳤶�

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
  GUI_BUTTON_SEND,   //���Ͱ�ť
  GUI_BUTTON_K0,     //K0����
  GUI_BUTTON_BACK,   //ɾ����ť
  GUI_BUTTON_MAX,    //��������
  GUI_BUTTON_NONE,   //��Ч����
}EnumGUIButtons;

typedef enum
{
  GUI_LINE_EDIT_SEND_DATA = 0, //�������ݣ������Ǵ�0��ʼ
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

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ButtonPressCallback(void* button);     //�����ص�����
static EnumGUIButtons ScanGUIButton(void);         //����ɨ��
static void LineEditPressCallback(void* lineEdit); //�б༭�ص�����
static void ScanGUILineEdit(void);                 //�б༭ģ��ɨ��
static u8   GetGUILineEditRusult(EnumGUILineEdit lineEdit, char** result); //��ȡ�б༭�ؼ�������
static void SendProc(char* string); //���ʹ�����
static void ReadProc(void);         //�����ڴ���

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
* ע    �⣺
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
* �������ƣ�GetGUILineEditRusult
* �������ܣ���ȡ�б༭�ؼ�������
* ���������lineEdit���ؼ�ö�٣�result�����ڷ���������
* ���������void
* �� �� ֵ��0-��ȡ�ɹ�����������ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u8 GetGUILineEditRusult(EnumGUILineEdit lineEdit, char** result)
{
  //�ؼ�������ɷ�Χ
  if(lineEdit >= GUI_LINE_EDIT_MAX)
  {
    result = NULL;
    return 1;
  }

  //��ǰ�б༭�������ݲ��Ҵ��ڻ�ý���״̬
  if((EDIT_LINE_HAS_FOCUS == s_arrLineEdit[lineEdit].hasFocus) && (0 != s_arrLineEdit[lineEdit].text[0]))
  {
    *result = s_arrLineEdit[lineEdit].text;
    return 0;
  }
  
  //�����ݻ�δ��ý���
  else
  {
    result = NULL;
    return 1;
  }
}

/*********************************************************************************************************
* �������ƣ�SendProc
* �������ܣ����ʹ�����
* ���������string����������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void SendProc(char* string)
{
  u8 i;        //���ͼ���
  u8 sendData; //��������

  //У�������ַ�Ƿ�Ϊ��ָ��
  if(NULL == string)
  {
    return;
  }

  //ͨ��UART5�����ݷ��ͳ�ȥ
  i = 0;
  while(0 != *(string + i))
  {
    //��ȡ��Ҫ���͵�����
    sendData = *(string + i);

    //ͨ�����ڷ��ͳ�ȥ
    WriteUART5(&sendData, 1);

    //���ͼ�����һ
    i++;
  }

  //���ͻس�����
  sendData = '\r';
  WriteUART5(&sendData, 1);
  sendData = '\n';
  WriteUART5(&sendData, 1);
}

/*********************************************************************************************************
* �������ƣ�ReadProc
* �������ܣ������ڴ���
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺ÿ��һ��ʱ�����һ��
*********************************************************************************************************/
static void ReadProc(void)
{
  static u8  s_arrReadBuf[48] = {0}; //���ջ�����
  static u8  s_iReadCnt       = 0;   //���ռ���
  static u64 s_iBeginTime     = 0;   //���յ���һ���ַ�ʱ��ϵͳʱ��

  u8  readData;
  u64 time;

  //��ȡϵͳ����ʱ��
  time = GUIGetSysTime();

  //�������ݴ���
  while(ReadUART5(&readData, 1))
  {
    //��¼���յ���һ���ַ�ʱ��ϵͳʱ��
    if(0 == s_iReadCnt)
    {
      s_iBeginTime = time;
    }

    //�����ݴ��浽������
    s_arrReadBuf[s_iReadCnt] = readData;

    //���ռ�����һ
    s_iReadCnt = (s_iReadCnt + 1) % (sizeof(s_arrReadBuf) / sizeof(u8));

    //���յ��س�����
    if(s_iReadCnt >= 2)
    {
      if(('\r' == s_arrReadBuf[s_iReadCnt - 2]) && ('\n' == s_arrReadBuf[s_iReadCnt - 1]))
      {
        //�����ַ�����β
        s_arrReadBuf[s_iReadCnt] = 0;

        //���ռ�������
        s_iReadCnt = 0;

        //������ն���ʾ
        TerminalWidgetShowLine(&s_structTerminal, (char*)s_arrReadBuf);
      }
    }
  }

  //����250���뻹û���յ��س�������ǿ�Ƹ��µ��ն���ʾ
  if((s_iReadCnt > 0) && ((time - s_iBeginTime) > 250))
  {
    //�����ַ�����β
    s_arrReadBuf[s_iReadCnt] = 0;

    //���ռ�������
    s_iReadCnt = 0;

    //������ն���ʾ
    TerminalWidgetShowLine(&s_structTerminal, (char*)s_arrReadBuf);
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUIRS232
* �������ܣ�����GUI RS232����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateGUIRS232(void)
{
  //����������
  static const u16 s_arrButtonX[GUI_BUTTON_MAX] = {560, 640, 720,  //K7-K9
                                                   560, 640, 720,  //K4-K5
                                                   560, 640, 720,  //K1-K3
                                                   560, 640, 720}; //SEND-BACK

  //����������
  static const u16 s_arrButtonY[GUI_BUTTON_MAX] = {220, 220, 220,  //K7-K9
                                                   285, 285, 285,  //K4-K5
                                                   350, 350, 350,  //K1-K3
                                                   415, 415, 415}; //SEND-BACK

  //������ʾ�ַ�
  static const char* s_arrButtonText[GUI_BUTTON_MAX] = {"7","8", "9", "4", "5", "6", "1", "2", "3", "Send", "0", "Back"};
  
  //�������¡�̧��ͼƬ
  static StructBmpImage s_structKeyPressImage;
  static StructBmpImage s_structKeyReleaseImage;

  //���������ͼƬ
  static StructJpegImage s_structHasFocusImage;
  static StructJpegImage s_structNoFocusImage;

  //�ն����ͼƬ
  static StructJpegImage s_structTerminalImage;

  u32 i;

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //��ʾ����ͼƬ
  DisPlayBMPByIPA(0, 0, RS232_BACKGROUND_DIR);

  //�������±���ͼƬ
  s_structKeyPressImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyPressImage.alphaGate   = 200;
  s_structKeyPressImage.storageType = BMP_IN_FATFS;
  s_structKeyPressImage.addr        = (void*)RS232_BUTTON_PRESS_DIR;

  //�����ͷű���ͼƬ
  s_structKeyReleaseImage.alphaType   = BMP_GATE_ALPHA;
  s_structKeyReleaseImage.alphaGate   = 200;
  s_structKeyReleaseImage.storageType = BMP_IN_FATFS;
  s_structKeyReleaseImage.addr        = (void*)RS232_BUTTON_RELEASE_DIR;

  //����ް�������
  s_enumButtonPress = GUI_BUTTON_NONE;

  //����K0-KF
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

  //�б༭�ؼ�
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

  //�ն˿ؼ�
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

  //��ʾ���ں�
  LCDShowString(620, 60, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "UART5");

  //��ʾ������
  LCDShowString(620, 80, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "115200");

  //����UART5
  InitUART5(115200);
}

/*********************************************************************************************************
* �������ƣ�DeleteGUIRS232
* �������ܣ�ע��GUI RS232����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUIRS232(void)
{
  DeInitUART5();
}

/*********************************************************************************************************
* �������ƣ�GUIRS232Poll
* �������ܣ�GUI RS232��ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIRS232Poll(void)
{
  const char      s_arrButtonValue[12] = {'7', '8', '9', '4', '5', '6', '1', '2', '3', 'S', '0', 'B'};
  EnumGUIButtons  button;
  EnumGUILineEdit lineEdit;
  char*           string = NULL;

  //����ɨ��
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //���ʹ���
    if(GUI_BUTTON_SEND == button)
    {
      if(0 == GetGUILineEditRusult(GUI_LINE_EDIT_SEND_DATA, &string))
      {
        SendProc(string);
      }
    }

    //ɾ������
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

    //K0-K9����
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
       
  //�б༭ģ��ɨ��
  ScanGUILineEdit();

  //�������ݴ���
  ReadProc();
}
