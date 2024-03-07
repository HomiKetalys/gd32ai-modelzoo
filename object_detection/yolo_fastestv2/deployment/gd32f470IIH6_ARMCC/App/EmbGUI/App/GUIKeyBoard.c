/*********************************************************************************************************
* ģ�����ƣ�GUIKeyBoard.c
* ժ    Ҫ������ģ��
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
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
//����ö��
typedef enum
{
  GUI_BUTTON_ESC = 0,   // ESC�������Ǵ�0��ʼ
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
  GUI_BUTTON_MAX,       // ��������
  GUI_BUTTON_NONE,      // ��Ч����
}EnumGUIButtons;

//����ɨ����
typedef struct
{
  u32 num;                               //����������������
  EnumGUIButtons button[GUI_BUTTON_MAX]; //����ɨ����������ǵ�һ�����ڶ�������n���������µļ�ֵ
}StructGUIButtonResult;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructButtonWidget    s_arrButton[GUI_BUTTON_MAX]; //��������
static StructGUIButtonResult s_structButtonPress;         //����ɨ����

//��ֵת����GUI�������ļ�ֵ��HID��ֵ��һ�£���Ҫ��ת��
//GUI��ֵ����Ǵ������ң��������£���0��ʼ���
static u8 s_arrKeyTable[] = 
{
  //��һ��
  KEYBOARD_ESC, KEYBOARD_1, KEYBOARD_2, KEYBOARD_3, KEYBOARD_4, KEYBOARD_5, KEYBOARD_6, KEYBOARD_7,
  KEYBOARD_8, KEYBOARD_9, KEYBOARD_0, KEYBOARD_MINUS, KEYBOARD_EQUAL, KEYBOARD_BACKSPACE,

  //�ڶ���
  KEYBOARD_TAB, KEYBOARD_Q, KEYBOARD_W, KEYBOARD_E, KEYBOARD_R, KEYBOARD_T, KEYBOARD_Y, KEYBOARD_U, 
  KEYBOARD_I, KEYBOARD_O, KEYBOARD_P, KEYBOARD_LPARAN, KEYBOARD_RPARAN, KEYBOARD_SLASH,

  //������
  KEYBOARD_CAPSLOCK, KEYBOARD_A, KEYBOARD_S, KEYBOARD_D, KEYBOARD_F, KEYBOARD_G, KEYBOARD_H, KEYBOARD_J,
  KEYBOARD_K, KEYBOARD_L, KEYBOARD_SEMICOLON, KEYBOARD_QUOTATION, KEYBOARD_ENTER,

  //������
  KEYBOARD_LEFT_SHIFT, KEYBOARD_Z, KEYBOARD_X, KEYBOARD_C, KEYBOARD_V, KEYBOARD_B, KEYBOARD_N, KEYBOARD_M,
  KEYBOARD_COMMA, KEYBOARD_PERIOD, KEYBOARD_QUESTION, KEYBOARD_RIGHT_SHIFT,

  //������
  KEYBOARD_LEFT_CTRL, KEYBOARD_LEFT_WIN, KEYBOARD_LEFT_ALT, KEYBOARD_SPACEBAR, KEYBOARD_RIGHT_ALT, 
  KEYBOARD_RIGHT_CTRL,
};

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ButtonPressCallback(void* button);     //�����ص�����
static StructGUIButtonResult* ScanGUIButton(void); //����ɨ��

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

  //��ȡ�������½��
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    if((StructButtonWidget*)button == &s_arrButton[i])
    {
      s_structButtonPress.button[s_structButtonPress.num++] = i;
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
static StructGUIButtonResult* ScanGUIButton(void)
{
  EnumGUIButtons i;

  //����ް�������
  s_structButtonPress.num = 0;
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    s_structButtonPress.button[i] = GUI_BUTTON_NONE;
  }

  //����ɨ�����а�������������ɨ��������s_structButtonPress��
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    ScanButtonWidget(&s_arrButton[i]);
  }

  return &s_structButtonPress;
}

/*********************************************************************************************************
* �������ƣ�KeyProc
* �������ܣ�����������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1�����¿��Ƽ���shift��ctrl��win����ǵ÷��Ϳհ���ָ���������ȡ�����Ƽ�״̬��������Ի�����Ϊ���Ƽ�һֱ
*             ���ڰ���״̬���������ظ��ŵ���������������
*          2��GUI����ɨ���⵽û�а�������ʱҲ�����һ�δ˻ص����������Է����û�������Ƽ�״̬
*********************************************************************************************************/
static void KeyProc(StructGUIButtonResult* result)
{
  u8 i, funcKey, key[6];

  //��ʼ����ֵ
  funcKey = 0;
  for(i = 0; i < 6; i++)
  {
    key[i] = 0;
  }

  //��ȡ��ֵ������ʱ������
  for(i = 0; i < result->num; i++)
  {
    //��Ctrl��
    if(GUI_BUTTON_LCTRL == result->button[i])
    {
      funcKey = funcKey | SET_LEFT_CTRL;
    }

    //��Ctrl��
    else if(GUI_BUTTON_RCTRL == result->button[i])
    {
      funcKey = funcKey | SET_RIGHT_CTRL;
    }

    //��Shift��
    else if(GUI_BUTTON_LSHIFT == result->button[i])
    {
      funcKey = funcKey | SET_LEFT_SHIFT;
    }

    //��Shift��
    else if(GUI_BUTTON_RSHIFT == result->button[i])
    {
      funcKey = funcKey | SET_RIGHT_SHIFT;
    }

    //��Alt��
    else if(GUI_BUTTON_LALT == result->button[i])
    {
      funcKey = funcKey | SET_LEFT_ALT;
    }

    //��Alt��
    else if(GUI_BUTTON_RALT == result->button[i])
    {
      funcKey = funcKey | SET_RIGHT_ALT;
    }

    //Windows��
    else if(GUI_BUTTON_WIN == result->button[i])
    {
      funcKey = funcKey | SET_LEFT_WINDOWS;
    }

    //��ֵת�������浽��ֵ������
    if(GUI_BUTTON_NONE != result->button[i])
    {
      key[i] = s_arrKeyTable[result->button[i]];
    }

    //ֻ��ȡǰ6����ֵ
    if(i >= 5)
    {
      break;
    }
  }
  
  //�ϱ���ֵ
  while(0 != SendKeyVal(funcKey, key[0], key[1], key[2], key[3], key[4], key[5]));
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUIKeyBoard
* �������ܣ���������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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

  //��ʾ�ַ�
  static const char* s_arrButtonText[GUI_BUTTON_MAX] = 
  {"ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "- -", "= +", "Back",   //��һ��
   "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[ {", "] }", "\\ |",   //�ڶ���
   "CapsLock", "A", "S", "D", "F", "G", "H", "J", "K", "L", "; :", "' \"", "Enter", //������
   "Shift", "Z", "X", "C", "V", "B", "N", "M", ", <", ". >", "/ ?", "Shift",        //������
   "Ctrl", "Win", "Alt", "", "Alt", "Ctrl"};                                        //������

  //�ؼ����
  static const u16 s_arrButtonWidth[GUI_BUTTON_MAX] = 
  {
    45 , 45, 45, 45 , 45, 45, 45, 45, 45, 45, 45, 45 , 45, 96, //��һ��
    70 , 45, 45, 45 , 45, 45, 45, 45, 45, 45, 45, 45 , 45, 70, //�ڶ���
    96 , 45, 45, 45 , 45, 45, 45, 45, 45, 45, 45, 45 , 96,     //������
    123, 45, 45, 45 , 45, 45, 45, 45, 45, 45, 45, 123,         //������
    96 , 45, 45, 416, 45, 96};                                 //������
  
  //�ַ�����������ڿؼ���ƫ����
  static const u8 s_arrStringX0[GUI_BUTTON_MAX] = 
  {
    4  , 16, 16, 16 , 16, 16, 16, 16, 16, 16, 16, 4 , 4 , 24, //��һ��
    17 , 16, 16, 16 , 16, 16, 16, 16, 16, 16, 16, 4 , 4 , 17, //�ڶ���
    0  , 16, 16, 16 , 16, 16, 16, 16, 16, 16, 4 , 4 , 18,     //������
    31 , 16, 16, 16 , 16, 16, 16, 16, 4 , 4 , 4 , 31,         //������
    24 , 4 , 4 , 0  , 4 , 24};                                //������

  EnumGUIButtons i;
  u32 x, y;

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //���Ʊ���
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

  //����ް�������
  s_structButtonPress.num = 0;
  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    s_structButtonPress.button[i] = GUI_BUTTON_NONE;
  }

  //��������
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

    //���ñ���ͼƬ
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

    //�����ؼ�
    CreateButtonWidget(&s_arrButton[i]);

    //���������
    x = x + s_arrButton[i].width + 8;

    //�л�����һ��
    if((GUI_BUTTON_BACK == i) || (GUI_BUTTON_SLASH == i) || (GUI_BUTTON_ENTER == i) || (GUI_BUTTON_RSHIFT == i) || (GUI_BUTTON_RCTRL == i))
    {
      x = 6;
      y = y + s_arrButton[0].height + 10;
    }
  }
  
  //��ʼ��USB��������
  InitKeyboard();

  //�ȴ��������
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
      SetInfoWidgetShow(&infoWidget, "�����²��USB");
      DelayNms(500);
    }
  }
  DeleteInfoWidget(&infoWidget);
  MyFree(SRAMEX, infoWidget.background);
}

/*********************************************************************************************************
* �������ƣ�DeleteGUIKeyBoard
* �������ܣ�ɾ������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUIKeyBoard(void)
{
  DeInitKeyboard();
}

/*********************************************************************************************************
* �������ƣ�GUIKeyBoardPoll
* �������ܣ�������ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIKeyBoardPoll(void)
{
  static u64 s_iSysLastTime = 0;
  static u64 s_iSysCurrentTime = 0;
  static u32 s_iLastKeyNum = 0;
  StructGUIButtonResult* button;

  //��ȡ��ǰʱ��
  s_iSysCurrentTime = GUIGetSysTime();

  if((s_iSysCurrentTime - s_iSysLastTime) >= KEY_BOARD_SCAN_TIME)
  {
    s_iSysLastTime = s_iSysCurrentTime;

    //����ɨ��
    button = ScanGUIButton();
    if((0 != button->num) || (s_iLastKeyNum != button->num))
    {
      KeyProc(button);
    }

    s_iLastKeyNum = button->num;
  }
}
