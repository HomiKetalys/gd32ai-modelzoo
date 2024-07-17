/*********************************************************************************************************
* ģ�����ƣ�Camera.c
* ժ    Ҫ������ͷģ��
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
#include "Camera.h"
#include "GUITop.h"
#include "stdio.h"
#include "TLILCD.h"
#include "OV2640.h"
#include "SysTick.h"
#include "BMPEncoder.h"
#include "Beep.h"
#include "DCI.h"
#include "RadioWidget.h"
#include "ButtonWidget.h"
#include "FontLib.h"
#include "Malloc.h"

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

#define IMAGE_SIZE     (IMAGE_WIDTH * IMAGE_HEIGHT * 2)

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static u32                s_iImageFrameAdd    = NULL;           //ͼ��֡�׵�ַ
//static StructRadioWidget  s_arrRadio[GUI_RADIO_MAX];            //��ѡ�ؼ�

static u8*                s_pStringBackground = NULL;           //�ַ�����ʾ����������
//static EnumGUIRadio       s_enumRadio         = GUI_RADIO_NONE; //��ǰѡ�еĿؼ�

static StructGUIDev s_structGUIDev;     //GUI�豸�ṹ��
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
static void PreviouCallback(EnumGUIRadio item);     //ǰһ��ص�����
static void NextCallback(EnumGUIRadio item);        //��һ��ص�����
static void RadioChangeCallback(EnumGUIRadio item); //��ѡ�л��ص�����
static void TakePhotoCallback(void);                //���ջص�����
/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
* �������ƣ�RadioChange
* �������ܣ���ѡ�л������ַ�����ʾ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void RadioChange(EnumGUIRadio item)
{
  //�л���ɫ�ȵ���
  if(GUI_RADIO_COLOR == item)
  {
    sprintf(s_arrStringBuf, "ɫ�ȣ�%+d", s_iColorSaturation);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //�л������ȵ���
  else if(GUI_RADIO_LIGHT == item)
  {
    sprintf(s_arrStringBuf, "���ȣ�%+d", s_iBrightness);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //�л����Աȶȵ���
  else if(GUI_RADIO_CONTRAST == item)
  {
    sprintf(s_arrStringBuf, "�Աȶȣ�%+d", s_iContrast);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //�л�����ƽ�����
  else if(GUI_RADIO_WB == item)
  {
    sprintf(s_arrStringBuf, "��ƽ�⣺%s", s_arrLmodeTable[s_iLightMode]);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //�л�����Ч����
  else if(GUI_RADIO_EFECT == item)
  {
    sprintf(s_arrStringBuf, "��Ч��%s", s_arrEffectTable[s_iEffects]);
    s_structGUIDev.showInfo(s_arrStringBuf);
  }

  //�����ʾ
  else
  {
    s_structGUIDev.showInfo("");
  }
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
static void PreviouCallback(EnumGUIRadio button)
{
  //ɫ�ȵ���
  if(GUI_RADIO_COLOR == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640ColorSaturation(s_iColorSaturation);
  }

  //���ȵ���
  else if(GUI_RADIO_LIGHT == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640Brightness(s_iBrightness);
  }

  //�Աȶȵ���
  else if(GUI_RADIO_CONTRAST == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640Contrast(s_iContrast);
  }

  //��ƽ�����
  else if(GUI_RADIO_WB == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640LightMode(s_iLightMode);
  }

  //��Ч����
  else if(GUI_RADIO_EFECT == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
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
static void NextCallback(EnumGUIRadio button)
{
  //ɫ�ȵ���
  if(GUI_RADIO_COLOR == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640ColorSaturation(s_iColorSaturation);
  }

  //���ȵ���
  else if(GUI_RADIO_LIGHT == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640Brightness(s_iBrightness);
  }

  //�Աȶȵ���
  else if(GUI_RADIO_CONTRAST == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640Contrast(s_iContrast);
  }

  //��ƽ�����
  else if(GUI_RADIO_WB == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640LightMode(s_iLightMode);
  }

  //��Ч����
  else if(GUI_RADIO_EFECT == button)
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
    s_structGUIDev.showInfo(s_arrStringBuf);
    OV2640SpecialEffects(s_iEffects);
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
static void TakePhotoCallback(void)
{
  //����������100ms
  BeepWithTime(100);

  //��ȡ��Ļͼ�񲢱��浽SD��
	ScreeShot();
}
/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitCamera
* �������ܣ���ʼ������ͷģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitCamera(void)
{
	//��ʼ������ͷ����
  s_iLightMode       = 0;
  s_iColorSaturation = 0;
  s_iBrightness      = 0;
  s_iContrast        = 0;
  s_iEffects         = 0;
	
	//��ʼ������ͷ
  if(InitOV2640() == 0)
  {
		//����OV2640
		DelayNms(50);
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
		InitDCI(s_iImageFrameAdd, IMAGE_SIZE, IMAGE_X0, IMAGE_Y0, IMAGE_WIDTH, IMAGE_HEIGHT, 0);
  }
  else
  {
    LCDShowString(30, 210, 200, 16, LCD_FONT_16,LCD_TEXT_NORMAL,LCD_COLOR_BLUE,LCD_COLOR_BLACK,"OV2640 Error!!");
    while(1);
  }
	
  //Ϊ�ַ������������ڴ�
  s_pStringBackground = MyMalloc(SDRAMEX, (STRING_X1 - STRIGN_X0) * STRING_SIZE * 2);
  if(NULL == s_pStringBackground)
  {
    while(1);
  }
	
	//���ûص�����
  s_structGUIDev.previousCallback    = PreviouCallback;
  s_structGUIDev.nextCallback        = NextCallback;
  s_structGUIDev.radioChangeCallback = RadioChange;
  s_structGUIDev.takePhotoCallback   = TakePhotoCallback;
  
  InitGUI(&s_structGUIDev);
}

/*********************************************************************************************************
* �������ƣ�CameraTask
* �������ܣ�����ͷģ������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CameraTask(void)
{
  //GUI����
  GUITask();

  //��ʾһ֡ͼ�ε���Ļ��
  DCIShowImage();
}
