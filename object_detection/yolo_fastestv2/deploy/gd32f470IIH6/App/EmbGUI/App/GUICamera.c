/*********************************************************************************************************
* ģ�����ƣ�GUICamera.c
* ժ    Ҫ�������ģ��
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
*                                              �궨��
*********************************************************************************************************/
//�ַ�����ʾ���
#define STRING_MAX_LEN (64)  //��ʾ�ַ���󳤶�
#define STRIGN_X0      (105) //��ʾ�ַ����������
#define STRING_Y0      (747) //��ʾ�ַ����������
#define STRING_X1      (373) //��ʾ�ַ��������յ�
#define STRING_SIZE    (24)  //�����С

//��Ƭ���
#define IMAGE_X0       (116)
#define IMAGE_Y0       (195)
#define IMAGE_WIDTH    (240)
#define IMAGE_HEIGHT   (320)
#define IMAGE_SIZE     (IMAGE_WIDTH * IMAGE_HEIGHT * 2)

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
typedef enum
{
  GUI_RADIO_COLOR = 0, //ɫ�ȣ������Ǵ�0��ʼ
  GUI_RADIO_LIGHT,     //����
  GUI_RADIO_CONTRAST,  //�Աȶ�
  GUI_RADIO_WB,        //��ƽ��
  GUI_RADIO_EFECT,     //��Ч
  GUI_RADIO_MAX,       //�ؼ�����
  GUI_RADIO_NONE,      //��Ч�ؼ�
}EnumGUIRadio;

typedef enum
{
  GUI_BUTTON_PREVIOUS = 0, //��һ�������������Ǵ�0��ʼ
  GUI_BUTTON_NEXT,         //��һ������
  GUI_BUTTON_PHOTO,        //���հ���
  GUI_BUTTON_MAX,          //��������
  GUI_BUTTON_NONE,         //��Ч����
}EnumGUIButtons;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructRadioWidget  s_arrRadio[GUI_RADIO_MAX];            //��ѡ�ؼ�
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX];          //��������
static u32                s_iImageFrameAdd    = NULL;           //ͼ��֡�׵�ַ
static u8*                s_pStringBackground = NULL;           //�ַ�����ʾ����������
static EnumGUIRadio       s_enumRadio         = GUI_RADIO_NONE; //��ǰѡ�еĿؼ�
static char               s_arrStringBuf[64];                   //�ַ���ת��������

//6�ֹ���ģʽ����ƽ�⣩
const char* s_arrLmodeTable[5]  = {"�Զ�", "����", "����", "�칫��", "����"};

//7����Ч
const char* s_arrEffectTable[7] = {"����", "��Ƭ", "�ڰ�", "ƫ��", "ƫ��", "ƫ��", "����"};

//����ͷ����
static i32 s_iLightMode       = 0; //��ƽ�⣨0-4��
static i32 s_iColorSaturation = 0; //ɫ�ȣ�-2~+2��
static i32 s_iBrightness      = 0; //���ȣ�-2~+2��
static i32 s_iContrast        = 0; //�Աȶȣ�-2~+2��
static i32 s_iEffects         = 0; //��Ч��0-6��

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�RadioPressCallback
* �������ܣ���ѡ�ص�����
* ���������radio�������µĵ�ѡ�ؼ���ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺���ҽ���һ����ѡ�ؼ��ܻ�ȡ����
*********************************************************************************************************/
static void RadioPressCallback(void* radio)
{
  u8 i;
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    if((StructRadioWidget*)radio != &s_arrRadio[i])
    {
      //ȡ���ؼ�����״̬
      ClearRadioWidgetFocus(&s_arrRadio[i]);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�ScanGUIRadio
* �������ܣ���ѡģ��ɨ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static EnumGUIRadio ScanGUIRadio(void)
{
  u32 i;
  EnumGUIRadio result;

  //ɨ��
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    ScanRadioWidget(&s_arrRadio[i]);
  }

  //��ȡ���н���Ŀؼ�ID
  result = GUI_RADIO_NONE;
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    if(RADIO_HAS_FOCUS == s_arrRadio[i].hasFocus)
    {
      result = (EnumGUIRadio)i;
      break;
    }
  }

  //����
  return result;
}

/*********************************************************************************************************
* �������ƣ�UpdataString
* �������ܣ������ַ�����ʾ
* ���������text���ַ���
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void UpdataString(char* text)
{
  u32 len, max, total, begin;

  //�����ַ�������
  len = 0;
  while((0 != text[len]) && ('\r' != text[len]) && ('\n' != text[len]))
  {
    len++;
  }

  //������Ļһ�������ʾ���ٸ��ַ�
  max = (STRING_X1 - STRIGN_X0) / 12;

  //��������ص�����
  total = 12 * len;

  //������ʼ��ʾ���ص�
  begin = STRIGN_X0 + ((STRING_X1 - STRIGN_X0) - total) / 2;

  //���Ʊ���ͼƬ
  if(NULL != s_pStringBackground)
  {
    GUIDrawBackground(STRIGN_X0, STRING_Y0, STRING_X1 - STRIGN_X0, STRING_SIZE, (u32)s_pStringBackground);
  }
  
  //��ʾ
  GUIDrawTextLine(begin, STRING_Y0, (u32)text, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1, max);
}

/*********************************************************************************************************
* �������ƣ�PreviouCallback
* �������ܣ�ǰһ��ص�����
* ���������button���ؼ��׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��һ��Ĭ�ϵݼ�����һ��Ĭ�ϵ���
*********************************************************************************************************/
static void PreviouCallback(void* button)
{
  //ɫ�ȵ���
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
    sprintf(s_arrStringBuf, "ɫ�ȣ�%+d", s_iColorSaturation);
    UpdataString(s_arrStringBuf);
    OV2640ColorSaturation(s_iColorSaturation);
  }

  //���ȵ���
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
    sprintf(s_arrStringBuf, "���ȣ�%+d", s_iBrightness);
    UpdataString(s_arrStringBuf);
    OV2640Brightness(s_iBrightness);
  }

  //�Աȶȵ���
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
    sprintf(s_arrStringBuf, "�Աȶȣ�%+d", s_iContrast);
    UpdataString(s_arrStringBuf);
    OV2640Contrast(s_iContrast);
  }

  //��ƽ�����
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
    sprintf(s_arrStringBuf, "��ƽ�⣺%s", s_arrLmodeTable[s_iLightMode]);
    UpdataString(s_arrStringBuf);
    OV2640LightMode(s_iLightMode);
  }

  //��Ч����
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
    sprintf(s_arrStringBuf, "��Ч��%s", s_arrEffectTable[s_iEffects]);
    UpdataString(s_arrStringBuf);
    OV2640SpecialEffects(s_iEffects);
  }
}

/*********************************************************************************************************
* �������ƣ�NextCallback
* �������ܣ���һ��ص�����
* ���������button���ؼ��׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��һ��Ĭ�ϵݼ�����һ��Ĭ�ϵ���
*********************************************************************************************************/
static void NextCallback(void* button)
{
  //ɫ�ȵ���
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
    sprintf(s_arrStringBuf, "ɫ�ȣ�%+d", s_iColorSaturation);
    UpdataString(s_arrStringBuf);
    OV2640ColorSaturation(s_iColorSaturation);
  }

  //���ȵ���
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
    sprintf(s_arrStringBuf, "���ȣ�%+d", s_iBrightness);
    UpdataString(s_arrStringBuf);
    OV2640Brightness(s_iBrightness);
  }

  //�Աȶȵ���
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
    sprintf(s_arrStringBuf, "�Աȶȣ�%+d", s_iContrast);
    UpdataString(s_arrStringBuf);
    OV2640Contrast(s_iContrast);
  }

  //��ƽ�����
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
    sprintf(s_arrStringBuf, "��ƽ�⣺%s", s_arrLmodeTable[s_iLightMode]);
    UpdataString(s_arrStringBuf);
    OV2640LightMode(s_iLightMode);
  }

  //��Ч����
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
    sprintf(s_arrStringBuf, "��Ч��%s", s_arrEffectTable[s_iEffects]);
    UpdataString(s_arrStringBuf);
    OV2640SpecialEffects(s_iEffects);
  }
}

/*********************************************************************************************************
* �������ƣ�RadioChange
* �������ܣ���ѡ�л������ַ�����ʾ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void RadioChange(void)
{
  //�л���ɫ�ȵ���
  if(GUI_RADIO_COLOR == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "ɫ�ȣ�%+d", s_iColorSaturation);
    UpdataString(s_arrStringBuf);
  }

  //�л������ȵ���
  else if(GUI_RADIO_LIGHT == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "���ȣ�%+d", s_iBrightness);
    UpdataString(s_arrStringBuf);
  }

  //�л����Աȶȵ���
  else if(GUI_RADIO_CONTRAST == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "�Աȶȣ�%+d", s_iContrast);
    UpdataString(s_arrStringBuf);
  }

  //�л�����ƽ�����
  else if(GUI_RADIO_WB == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "��ƽ�⣺%s", s_arrLmodeTable[s_iLightMode]);
    UpdataString(s_arrStringBuf);
  }

  //�л�����Ч����
  else if(GUI_RADIO_EFECT == s_enumRadio)
  {
    sprintf(s_arrStringBuf, "��Ч��%s", s_arrEffectTable[s_iEffects]);
    UpdataString(s_arrStringBuf);
  }

  //�����ʾ
  else
  {
    UpdataString("");
  }
}

/*********************************************************************************************************
* �������ƣ�TakePhotoCallback
* �������ܣ����ջص�����
* ���������button���ؼ��׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void TakePhotoCallback(void* button)
{
  //����������100ms
  BeepWithTime(100);

  //��ȡ��Ļͼ�񲢱��浽SD��
  BMPEncodeWithRGB565(IMAGE_X0, IMAGE_Y0, IMAGE_WIDTH, IMAGE_HEIGHT, "0:/photo", "OV2640Image");
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUICamera
* �������ܣ����������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺����ʱ�����ǰ���㣬ͬʱDCIʹ��ģʽ0��DMAֱ�ӽ�DCI���ݴ��䵽�ִ��У�ˢ���ٶȿ�
*           ��LCDˢ����Ҫ���ͣ������п��ܻ���ֻ���
*********************************************************************************************************/
void CreateGUICamera(void)
{
  //��ѡ�ؼ�����
  static const u16 s_arrX0[GUI_RADIO_MAX] = {43, 204, 365, 42 , 365};  //������
  static const u16 s_arrY0[GUI_RADIO_MAX] = {17, 20 , 20  , 645, 645}; //������
  u32 i;

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //���Ʊ���
  DisPlayBMPByIPA(0, 0, CAMERA_BACKGROUND_DIR);

  //ͼ����ʾ�������Ϊ��ɫ
  GUIFillColor(IMAGE_X0, IMAGE_Y0, IMAGE_X0 + IMAGE_WIDTH - 1, IMAGE_Y0 + IMAGE_HEIGHT - 1, GUI_COLOR_BLACK);

  //������һ�ť
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

  //������һ�ť
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

  //�������հ�ť
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

  //������ѡ�ؼ�
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

  //Ϊ�ַ������������ڴ�
  s_pStringBackground = MyMalloc(SRAMEX, (STRING_X1 - STRIGN_X0) * STRING_SIZE * 2);
  if(NULL == s_pStringBackground)
  {
    while(1);
  }

  //�����ַ�������
  GUISaveBackground(STRIGN_X0, STRING_Y0, STRING_X1 - STRIGN_X0, STRING_SIZE, (u32)s_pStringBackground);

  //����OV2640
  InitOV2640();
  OV2640RGB565Mode();
  OV2640OutSizeSet(IMAGE_WIDTH, IMAGE_HEIGHT);
  OV2640AutoExposure(0);
  OV2640LightMode(s_iLightMode);
  OV2640ColorSaturation(s_iColorSaturation);
  OV2640Brightness(s_iBrightness);
  OV2640Contrast(s_iContrast);
  OV2640SpecialEffects(s_iEffects);

  //����ͼ���Դ��ַΪǰ�����׵�ַ
  s_iImageFrameAdd = g_structTLILCDDev.foreFrameAddr;

  //����DCI
  InitDCI(s_iImageFrameAdd, IMAGE_SIZE, IMAGE_X0, IMAGE_Y0, IMAGE_WIDTH, IMAGE_HEIGHT, 1);
}

/*********************************************************************************************************
* �������ƣ�DeleteGUICamera
* �������ܣ�ɾ�������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUICamera(void)
{
  u32 i;

  //��λDCI
  DeInitDCI();

  //����ͷ�������ģʽ
  OV2640PWDNSet(1);

  //����ǰ����
  //LCDLayerDisable(LCD_LAYER_FOREGROUND);

  //�ͷŵ�ѡ�ؼ�����Ķ�̬�ڴ棬��ɾ���ؼ�
  for(i = 0; i < GUI_RADIO_MAX; i++)
  {
    MyFree(SRAMEX, s_arrRadio[i].background);
    DeleteRadioWidget(&s_arrRadio[i]);
  }

  //ɾ�������ؼ�
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    DeleteButtonWidget(&s_arrButton[i]);
  }
}

/*********************************************************************************************************
* �������ƣ�GUICameraPoll
* �������ܣ��������ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUICameraPoll(void)
{
  EnumGUIRadio radio;
  u32 i;

  //��ʾһ֡ͼ�ε���Ļ��
  DCIShowImage();

  //��ѡ�ؼ�ɨ��
  radio = ScanGUIRadio();
  if(s_enumRadio != radio)
  {
    s_enumRadio = radio;
    RadioChange();
  }

  //����ɨ��
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    ScanButtonWidget(&s_arrButton[i]);
  }
}
