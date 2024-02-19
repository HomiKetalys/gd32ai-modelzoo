/*********************************************************************************************************
* ģ�����ƣ�GUIHome.c
* ժ    Ҫ��������
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
#include "GUIHome.h"
#include "TLILCD.h"
#include "GUIPlatform.h"
#include "BMP.h"
#include "Common.h"
#include "GUITop.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define ICON_IMAGE_WIDTH  (80)                //ͼ����
#define ICON_IMAGE_HEIGHT (80)                //ͼ��߶�
#define ICON_TEXT_FONT    (GUI_FONT_ASCII_24) //ͼ������
#define ICON_TEXT_SPACE   (2)                 //�ַ�����ͼƬ�ļ�϶
#define ICON_APP_SPAGE    ((480 - ICON_IMAGE_WIDTH * 4) / 5)

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
//������ͼ��
typedef struct
{
  EnumGUIList     app;    //����App
  u32             x0;     //�������
  u32             y0;     //�������
  u32             width;  //���
  u32             height; //�߶�
  StructBmpImage* image;  //ͼ��ͼƬ��ʹ��λͼ�ļ���û�ж���ʱĬ��ʹ����ɫ���
  const char*     text;   //App���⣬��Ĭ��ʹ��24x24����
}StructAppIcon;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructAppIcon  s_arrAppIcon[GUI_LIST_MAX - 1];  //APPͼ�꼯��
static StructBmpImage s_arrAppImage[GUI_LIST_MAX - 1]; //ͼ��λͼ

//App����
static const char* s_arrAppTitle[GUI_LIST_MAX - 1] = 
{
  "ʱ��",
  "RS232",
  "RS485",
  "CAN",
  "����",
  "�ļ�",
  "��̫��",
  "����",
  "¼����",
  "�����",
  "�Ķ�",
  "����",
  "Wave",
  "��ʪ��",
  "������",
  "OLED",
};

//Appͼ��
static const char* s_arrImageAddr[GUI_LIST_MAX - 1] = 
{
  CLOCK_ICON_DIR,
  RS232_ICON_DIR,
  RS485_ICON_DIR,
  CAN_ICON_DIR,
  KEYBOARD_ICON_DIR,
  FILESYS_ICON_DIR,
  ETHERNET_ICON_DIR,
  MUSIC_ICON_DIR,
  RECORDER_ICON_DIR,
  CAMERA_ICON_DIR,
  READER_ICON_DIR,
  CANVAS_ICON_DIR,
  WAVE_ICON_DIR,
  TEMP_ICON_DIR,
  BREATH_LED_ICON_DIR,
  OLED_ICON_DIR,
};

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void DisplayBackground(void);           //���Ʊ���
static void CreateAppIcon(StructAppIcon* app); //����Appͼ��
static void InitAllAppStruct(void);            //��ʼ������Appͼ��
static StructAppIcon* ScanAppIcon(void);       //ɨ��Appͼ��

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
  DisPlayBMPByIPA(0, 0, MAIN_WINDOWN_BACKGROUND_DIR);
}

/*********************************************************************************************************
* �������ƣ�CreateAppIcon
* �������ܣ�����Appͼ��
* ���������app��Appͼ��ṹ���ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void CreateAppIcon(StructAppIcon* app)
{
  u32 stringNum;      //�ַ����ַ���
  u32 stringLen;      //�ַ������ȣ����ص㣩
  u32 beginX, beginY; //�ַ����������
  u32 x0, y0, x1, y1;

  //��ʾͼƬ
  x0 = app->x0;
  y0 = app->y0;
  x1 = app->x0 + ICON_IMAGE_WIDTH - 1;
  y1 = app->y0 + ICON_IMAGE_HEIGHT - 1;
  if(NULL != app->image)
  {
    if(NULL != app->image->addr)
    {
      GUIDrawImage(app->x0, app->y0, (u32)app->image, GUI_IMAGE_TYPE_BMP);
    }
    else
    {
      GUIFillColor(x0, y0, x1, y1, GUI_COLOR_BLUE);
    }
  }
  else
  {
    GUIFillColor(x0, y0, x1, y1, GUI_COLOR_BLUE);
  }

  //ͳ���ַ����ܳ���
  stringNum = 0;
  while(0 != app->text[stringNum])
  {
    stringNum++;
  }

  //У���ַ������ȣ�App���ⲻ�ܳ���ͼƬ���
  if(stringNum > (app->width / GetFontWidth(ICON_TEXT_FONT)))
  {
    stringNum = app->width / GetFontWidth(ICON_TEXT_FONT);
  }

  //�����ַ������ȣ����ص㣩
  stringLen = stringNum * GetFontWidth(ICON_TEXT_FONT);
  
  //������ʼ������
  beginY = app->y0 + ICON_IMAGE_HEIGHT + ICON_TEXT_SPACE;

  //������ʼ������
  beginX = app->x0 + (app->width - stringLen) / 2;

  //��ʾApp����
  GUIDrawTextLine(beginX, beginY, (u32)app->text, ICON_TEXT_FONT, NULL, GUI_COLOR_WHITE, 1, stringNum);
}

/*********************************************************************************************************
* �������ƣ�InitAllAppStruct
* �������ܣ���ʼ������Appͼ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void InitAllAppStruct(void)
{
  u32 i, x, y;

  //��ʼ��Appλͼ����
  for(i = 0; i < GUI_LIST_MAX - 1; i++)
  {
    s_arrAppImage[i].storageType = BMP_IN_FATFS;
    s_arrAppImage[i].alphaType = BMP_UES_ALPHA;
    s_arrAppImage[i].addr = (void*)s_arrImageAddr[i];
  }

  //��ʼ��Appͼ��
  x = ICON_APP_SPAGE;
  y = ICON_APP_SPAGE;
  for(i = 0; i < GUI_LIST_MAX - 1; i++)
  {
    //����y��
    if((0 == (i % 4)) && (0 != i))
    {
      y = y + s_arrAppIcon[0].height + ICON_APP_SPAGE;
      x = ICON_APP_SPAGE;
    }

    s_arrAppIcon[i].app = (EnumGUIList)(i + 1);
    s_arrAppIcon[i].x0 = x;
    s_arrAppIcon[i].y0 = y;
    s_arrAppIcon[i].width = ICON_IMAGE_WIDTH;
    s_arrAppIcon[i].height = ICON_IMAGE_HEIGHT + ICON_TEXT_SPACE + GetFontHeight(ICON_TEXT_FONT);
    s_arrAppIcon[i].image = &s_arrAppImage[i];
    s_arrAppIcon[i].text = s_arrAppTitle[i];

    //���º�����
    x = x + ICON_IMAGE_WIDTH + ICON_APP_SPAGE;
  }
}

/*********************************************************************************************************
* �������ƣ�ScanAppIcon
* �������ܣ�ɨ��Appͼ��
* ���������void
* ���������void
* �� �� ֵ�����µ�ͼ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static StructAppIcon* ScanAppIcon(void)
{
  u32 i, x, y, x0, y0, x1, y1;

  //��ȡ��������
  if(1 == GUIGetTouch(&x, &y, 0))
  {
    for(i = 0; i < GUI_LIST_MAX - 1; i++)
    {
      x0 = s_arrAppIcon[i].x0;
      y0 = s_arrAppIcon[i].y0;
      x1 = s_arrAppIcon[i].x0 + s_arrAppIcon[i].width - 1;
      y1 = s_arrAppIcon[i].y0 + s_arrAppIcon[i].height - 1;
      if((x >= x0) && (x <= x1) && (y >= y0) && (y <= y1))
      {
        return &s_arrAppIcon[i];
      }
    }
    return NULL;
  }

  //�޴��㰴�£�ֱ�ӷ���
  else
  {
    return NULL;
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUIHome
* �������ܣ�����Home����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateGUIHome(void)
{
  u32 i;

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //���Ʊ���
  DisplayBackground();

  //��ʼ��ͼ�����
  InitAllAppStruct();

  //��������ͼ��
  for(i = 0; i < GUI_LIST_MAX - 1; i++)
  {
    CreateAppIcon(&s_arrAppIcon[i]);
  }
}

/*********************************************************************************************************
* �������ƣ�DeleteGUIHome
* �������ܣ�ɾ��������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUIHome(void)
{

}

/*********************************************************************************************************
* �������ƣ�GUIHomePoll
* �������ܣ���������ѯ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIHomePoll(void)
{
  StructAppIcon* icon;
  icon = ScanAppIcon();
  if(NULL != icon)
  {
    printf("�л�����%s����\r\n", icon->text);
    GUISwitch(icon->app);
  }
}
