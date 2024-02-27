/*********************************************************************************************************
* ģ�����ƣ�GUITop.c
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
#include "GUITop.h"
#include "GUIPlatform.h"
#include "RadioWidget.h"
#include "ButtonWidget.h"
#include "TLILCD.h"
#include "BMP.h"
#include "JPEG.h"
#include "stdio.h"
#include "OV2640.h"
#include "DCI.h"
#include "Malloc.h"
#include "Beep.h"
#include "BMPEncoder.h"
#include "GUIRadio.h"
#include "GUIButton.h"

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
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructGUIDev* 			s_pGUIDev 					= NULL;
static u8*                s_pStringBackground = NULL;           //�ַ�����ʾ����������
static EnumGUIRadio       s_enumRadio         = GUI_RADIO_NONE; //��ǰѡ�еĿؼ�

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�DisplayBackground
* �������ܣ����Ʊ���
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺����֮ǰҪȷ��LCD����Ϊ����
*********************************************************************************************************/
static void DisplayBackground(void)
{
  //����ͼƬ���ƽṹ��
  StructJpegImage backgroundImage;

  //��ʼ��backgroundImage
  backgroundImage.image = (unsigned char*)s_arrJpegBackgroundImage;
  backgroundImage.size  = sizeof(s_arrJpegBackgroundImage) / sizeof(unsigned char);

  //���벢��ʾͼƬ
  DisplayJPEGInFlash(&backgroundImage, 0, 0);
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
*                                              API����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
* �������ƣ�InitGUI
* �������ܣ���ʼ��GUI
* ���������dev��GUI�豸�ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺����ʹ��12x24����
*********************************************************************************************************/
void InitGUI(StructGUIDev* dev)
{

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //���Ʊ���
  DisplayBackground();
	  
	//������ѡģ��
  InitGUIRadio();

  //��������
  InitGUIButton();
	
	//�����豸ָ��
  s_pGUIDev = dev;
	
	//Ĭ���޵�ѡ�ؼ�ѡ��
  s_enumRadio = GUI_RADIO_NONE;

  //ͼ����ʾ�������Ϊ��ɫ
  GUIFillColor(IMAGE_X0, IMAGE_Y0, IMAGE_X0 + IMAGE_WIDTH - 1, IMAGE_Y0 + IMAGE_HEIGHT - 1, GUI_COLOR_BLACK);

  //Ϊ�ַ������������ڴ�
  s_pStringBackground = MyMalloc(SDRAMEX, (STRING_X1 - STRIGN_X0) * STRING_SIZE * 2);
  if(NULL == s_pStringBackground)
  {
    while(1);
  }
	
  //�����ַ�������
  GUISaveBackground(STRIGN_X0, STRING_Y0, STRING_X1 - STRIGN_X0, STRING_SIZE, (u32)s_pStringBackground);
	
	//�����ַ������º���ָ��
  s_pGUIDev->showInfo = UpdataString;
	
	//�����ʾ
  UpdataString("");
	
}


/*********************************************************************************************************
* �������ƣ�GUITask
* �������ܣ��������ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUITask()
{
  EnumGUIRadio radio;
  EnumGUIButtons button;

  //��ѡ�ؼ�ɨ��
  radio = ScanGUIRadio();
  if(s_enumRadio != radio)
  {
    s_enumRadio = radio;
    if(NULL != s_pGUIDev->radioChangeCallback)
    {
      s_pGUIDev->radioChangeCallback(s_enumRadio);
    }
    return;
  }

  //����ɨ��
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //��һ��
    if(GUI_BUTTON_PREVIOUS == button)
    {
      if((NULL != s_pGUIDev->previousCallback) && (GUI_RADIO_NONE != s_enumRadio))
      {
        s_pGUIDev->previousCallback(s_enumRadio);
      }
    }

    //��һ��
    else if((GUI_BUTTON_NEXT == button) && (GUI_RADIO_NONE != s_enumRadio))
    {
      if(NULL != s_pGUIDev->nextCallback)
      {
        s_pGUIDev->nextCallback(s_enumRadio);
      }
    }

    //����
    else if(GUI_BUTTON_PHOTO == button)
    {
      if(NULL != s_pGUIDev->takePhotoCallback)
      {
        s_pGUIDev->takePhotoCallback();
      }
    }
  }
}
