/*********************************************************************************************************
* ģ�����ƣ�GUICAN.c
* ժ    Ҫ��GUI CANͨ��ģ��
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
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
typedef enum
{
  GUI_BUTTON_K0 = 0, //K0�����������Ǵ�0��ʼ
  GUI_BUTTON_K1,     //K1����
  GUI_BUTTON_K2,     //K2����
  GUI_BUTTON_K3,     //K3����
  GUI_BUTTON_K4,     //K4����
  GUI_BUTTON_K5,     //K5����
  GUI_BUTTON_K6,     //K6����
  GUI_BUTTON_K7,     //K7����
  GUI_BUTTON_K8,     //K8����
  GUI_BUTTON_K9,     //K9����
  GUI_BUTTON_KA,     //KA����
  GUI_BUTTON_KB,     //KB����
  GUI_BUTTON_KC,     //KC����
  GUI_BUTTON_KD,     //KD����
  GUI_BUTTON_KE,     //KE����
  GUI_BUTTON_KF,     //KF����
  GUI_BUTTON_SEND,   //���Ͱ���
  GUI_BUTTON_BACK,   //���ذ���
  GUI_BUTTON_MAX,    //��������
  GUI_BUTTON_NONE,   //��Ч����
}EnumGUIButtons;

typedef enum
{
  GUI_LINE_EDIT_SEND_ID = 0, //����ID
  GUI_LINE_EDIT_SEND_DATA,   //�������ݣ������Ǵ�0��ʼ
  GUI_LINE_EDIT_MAX,         //�ؼ�����
  GUI_LINE_EDIT_NONE,        //��Ч�ؼ�
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
static void ButtonPressCallback(void* button);       //�����ص�����
static EnumGUIButtons ScanGUIButton(void);           //����ɨ��
static void LineEditPressCallback(void* lineEdit);   //�б༭�ص�����
static u32  CharToU32(char c);                       //�ַ�תint
static void ScanGUILineEdit(void);                   //�б༭ģ��ɨ��
static u8   GetGUILineEditIDResult(u32* result);     //��ȡ����ID������
static u8   GetGUILineEditDataResult(char** result); //��ȡ����ID������
static void SendProc(u32 id, char* data);            //���ʹ�����
static void ReadProc(void);                          //�����ڴ���

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
* �������ƣ�CharToU32
* �������ܣ��ַ�תint
* ���������c����Ҫת�����ַ�
* ���������void
* �� �� ֵ���ַ���Ӧ����ֵ������16��ʾ����
* �������ڣ�2021��07��01��
* ע    �⣺���ҽ���һ���б༭�ؼ��ܻ�ȡ����
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
* �������ƣ�GetGUILineEditIDResult
* �������ܣ���ȡ����ID������
* ���������result�����ڷ���������
* ���������void
* �� �� ֵ��0-��ȡ�ɹ�����������ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺�б༭��Ϊ10��������
*********************************************************************************************************/
static u8 GetGUILineEditIDResult(u32* result)
{
  u32 sum;       //���ս��
  u32 charValue; //��ǰ�ַ���Ӧ����ֵ
  u8 i = 0;

  //�����б༭����ֵ
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

  //��ȡ��һ���ַ���Ӧ����ֵʱʧ�ܣ���ʾ��ǰ�б༭����Ϊ��
  if(0 == i)
  {
    return 1;
  }

  //��������ֵ
  else
  {
    *result = sum;
    return 0;
  }
}

/*********************************************************************************************************
* �������ƣ�GetGUILineEditIDResult
* �������ܣ���ȡ����ID������
* ���������result�����ڷ���������
* ���������void
* �� �� ֵ��0-��ȡ�ɹ�����������ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺�б༭��Ϊ10��������
*********************************************************************************************************/
static u8 GetGUILineEditDataResult(char** result)
{
  //�б༭��������
  if(0 != s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].text[0])
  {
    *result = s_arrLineEdit[GUI_LINE_EDIT_SEND_DATA].text;
    return 0;
  }
  
  //������
  else
  {
    result = NULL;
    return 1;
  }
}

/*********************************************************************************************************
* �������ƣ�SendProc
* �������ܣ����ʹ�����
* ���������id������ID��data����������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��Ϊ����ֻ��0-9������ID�������10�������ݣ������������ַ���
*********************************************************************************************************/
static void SendProc(u32 id, char* data)
{
  u8 i;        //���ͼ���
  u8 sendData; //��������

  //У�������ַ�Ƿ�Ϊ��ָ��
  if(NULL == data)
  {
    return;
  }

  //����������
  i = 0;
  while(0 != *(data + i))
  {
    i++;
  }

  //ͨ��CAN0�����ݷ��ͳ�ȥ
  WriteCAN0(id, (u8*)data, i);

  //���ͻس�����
  sendData = '\r';
  WriteCAN0(id, &sendData, 1);
  sendData = '\n';
  WriteCAN0(id, &sendData, 1);
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
  static char s_arrReadBuf[48] = {0}; //���ջ�����
  static u8   s_iReadCnt       = 0;   //���ռ���
  static u64  s_iBeginTime     = 0;   //���յ���һ���ַ�ʱ��ϵͳʱ��

  u8  readData;
  u64 time;

  //��ȡϵͳ����ʱ��
  time = GUIGetSysTime();  //ms

  //�������ݴ���
  while(ReadCAN0(&readData, 1))
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
        TerminalWidgetShowLine(&s_structTerminal, s_arrReadBuf);
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
    TerminalWidgetShowLine(&s_structTerminal, s_arrReadBuf);
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUICAN
* �������ܣ�����GUI CAN����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //��ʾ����ͼƬ
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

  //����ް�������
  s_enumButtonPress = GUI_BUTTON_NONE;

  //����K0-KF
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

  //������ȡ��ť
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

  //�������ذ�ť
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

  //��ʼ��JPEGͼƬ
  s_structHasFocusImage.image = (unsigned char*)CAN_LINE_EDIT_HAS_FOCUS_DIR;
  s_structNoFocusImage.image = (unsigned char*)CAN_LINE_EDIT_NO_FOCUS_DIR;

  //����ID
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

  //��������
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

  //�ն˿ؼ�
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

  //CONX��ʾ
  LCDShowString(545, 58, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "CAN0");

  //�ٶ���ʾ
  LCDShowString(545, 83, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "1MBps");

  //��ʶ����ʾ
  LCDShowString(690, 58, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "0x5A5");

  //������ʾ
  LCDShowString(690, 83, 100, 16, LCD_FONT_16, LCD_TEXT_TRANS, GUI_COLOR_WHITE, NULL, "0xFFFFFFFF");

  //��ʼ��CANģ��
  InitCAN();
}

/*********************************************************************************************************
* �������ƣ�DeleteGUICAN
* �������ܣ�ע��GUI CAN����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUICAN(void)
{
  //ע��CANģ��
  DeInitCAN();
}

/*********************************************************************************************************
* �������ƣ�GUICANPoll
* �������ܣ�GUI CAN��ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUICANPoll(void)
{
  const char s_arrButtonValue[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  EnumGUIButtons button;
  EnumGUILineEdit lineEdit;
  char* data = NULL;
  u32   id   = 0;

  //�б༭ģ��ɨ��
  ScanGUILineEdit();

  //����ɨ��
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //���ʹ���
    if(GUI_BUTTON_SEND == button)
    {
      if((0 == GetGUILineEditDataResult(&data)) && (0 == GetGUILineEditIDResult(&id)))
      {
        SendProc(id, data);
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

    //K0-KF����
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

  //�������ݲ����µ��ն���ʾ
  ReadProc();
}
