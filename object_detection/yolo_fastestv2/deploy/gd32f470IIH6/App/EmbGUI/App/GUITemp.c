/*********************************************************************************************************
* ģ�����ƣ�GUITemp.c
* ժ    Ҫ����ʪ����ʾģ��
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
#include "GUITemp.h"
#include "TLILCD.h"
#include "Common.h"
#include "BMP.h"
#include "GUIPlatform.h"
#include "TextWidget.h"
#include "stdio.h"
#include "math.h"
#include "InTemp.h"
#include "SHT20.h"
#include "ADC.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define SHOW_FLAG 0

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructTextWidget s_structInTemp;
static StructTextWidget s_structExTemp;
static StructTextWidget s_structHumidity;
static u16 s_arrInTempBackgound[1440];
static u16 s_arrExTempBackgound[1440];
static u16 s_arrHumidityBackgound[1440];

static u32 s_iTempColor;
static u32 s_iHumidityColor;
static u16 s_iHumidityBackground[106 * 106];

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void UpdataInTempShow(double temp);         //�����ڲ��¶�ֵ��ʾ
static void UpdataExTempShow(double temp);         //�����ⲿ�¶�ֵ��ʾ
static void UpdateExHumidityShow(double humidity); //�����ⲿʪ�ȶ�ֵ��ʾ

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�UpdataInTempShow
* �������ܣ������ڲ��¶�ֵ��ʾ
* ���������temp���¶�ֵ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void UpdataInTempShow(double temp)
{
  //�������
  u32 x0, y0, x1, y1, yTemp;

  //�ַ���ת��������
  char string[16];

  //�¶�ֵ��ʾ
  sprintf(string, "%.1f", temp);
  s_structInTemp.setText(&s_structInTemp, string);

  //�����������
  x0 = 115;
  y0 = 105;
  x1 = 128;
  y1 = 280;

  //0-50��
#if (0 == SHOW_FLAG)
  if((temp >= 0) && (temp <= 50))
  {
    yTemp = y1 - (y1 - y0) * temp / 50.0;
    GUIFillColor(x0, yTemp, x1, y1, s_iTempColor);
    GUIFillColor(x0, y0, x1, yTemp, GUI_COLOR_WHITE);
  }
  else
  {
    GUIFillColor(x0, y0, x1, y1, GUI_COLOR_WHITE);
  }
#endif

  //��50��
#if (1 == SHOW_FLAG)
  if((temp >= -50) && (temp <= 50))
  {
    yTemp = y1 - (y1 - y0) * (temp + 50.0) / 100.0;
    GUIFillColor(x0, yTemp, x1, y1, s_iTempColor);
    GUIFillColor(x0, y0, x1, yTemp, GUI_COLOR_WHITE);
  }
  else
  {
    GUIFillColor(x0, y0, x1, y1, GUI_COLOR_WHITE);
  }
#endif
}

/*********************************************************************************************************
* �������ƣ�UpdataExTempShow
* �������ܣ������ⲿ�¶�ֵ��ʾ
* ���������temp���¶�ֵ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void UpdataExTempShow(double temp)
{
  //�������
  u32 x0, y0, x1, y1, yTemp;

  //�ַ���ת��������
  char string[16];

  //�¶�ֵ��ʾ
  sprintf(string, "%.1f", temp);
  s_structExTemp.setText(&s_structExTemp, string);

  //�����������
  x0 = 386;
  y0 = 105;
  x1 = 399;
  y1 = 280;

  //0-50��
#if (0 == SHOW_FLAG)
  if((temp >= 0) && (temp <= 50))
  {
    yTemp = y1 - (y1 - y0) * temp / 50.0;
    GUIFillColor(x0, yTemp, x1, y1, s_iTempColor);
    GUIFillColor(x0, y0, x1, yTemp, GUI_COLOR_WHITE);
  }
  else
  {
    GUIFillColor(x0, y0, x1, y1, GUI_COLOR_WHITE);
  }
#endif

  //��50��
#if (1 == SHOW_FLAG)
  if((temp >= -50) && (temp <= 50))
  {
    yTemp = y1 - (y1 - y0) * (temp + 50.0) / 100.0;
    GUIFillColor(x0, yTemp, x1, y1, s_iTempColor);
    GUIFillColor(x0, y0, x1, yTemp, GUI_COLOR_WHITE);
  }
  else
  {
    GUIFillColor(x0, y0, x1, y1, GUI_COLOR_WHITE);
  }
#endif
}

/*********************************************************************************************************
* �������ƣ�UpdateExHumidityShow
* �������ܣ������ⲿʪ�ȶ�ֵ��ʾ
* ���������humidity��ʪ��ֵ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void UpdateExHumidityShow(double humidity)
{
  int x, y, x0, y0, x1, y1;
  double pi, angle0, angle1, angle;

  //�ַ���ת��������
  char string[16];

  //�¶�ֵ��ʾ
  if(humidity >= 100.0)
  {
    sprintf(string, "100");
  }
  else
  {
    sprintf(string, "%.1f", humidity);
  }
  s_structHumidity.setText(&s_structHumidity, string);

  //�ػ汳��
  GUIDrawBackground(616, 167, 106, 106, (u32)s_iHumidityBackground);

  //��ʾ��Χ�޶�
  if(humidity < 0)
  {
    humidity = 0;
  }
  else if(humidity > 100)
  {
    humidity = 100;
  }

  //��ʪ�ȷ�ת
  humidity = 100.0 - humidity;

  //��ֱ����ֱ������ϵ�еĽǶ�
  pi = 3.1415926535;
  angle0 = -pi / 3.0;      //100%��Ӧ�ĽǶȣ����ȣ�
  angle1 = pi * 4.0 / 3.0; //0%��Ӧ�ĽǶȣ����ȣ�
  angle = angle0 + (angle1 - angle0) * humidity / 100.0;

  //����
  x = 50.0 * cos(angle);
  y = 50.0 * sin(angle);
  y = -y;
  x0 = 669;
  y0 = 220;
  x1 = x0 + x;
  y1 = y0 + y;
  GUIDrawLine(x0, y0, x1, y1, 3, s_iHumidityColor, GUI_LINE_SQUARE);
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUITemp
* �������ܣ�������ʪ����ʾ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateGUITemp(void)
{
  //�ַ���ת��������
  char string[16];

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //���Ʊ���
  DisPlayBMPByIPA(0, 0, TEMP_BACKGROUND_DIR);

  //��ʼ����ʾģ��
  s_iTempColor = GUIReadPoint(122, 290);
  GUIFillColor(115, 105, 128, 280, GUI_COLOR_WHITE);
  GUIFillColor(386, 105, 399, 280, GUI_COLOR_WHITE);

  s_iHumidityColor = GUIReadPoint(669, 220);
  GUISaveBackground(616, 167, 106, 106, (u32)s_iHumidityBackground);

  //�ڲ��¶�
  InitTextWidgetStruct(&s_structInTemp);
  s_structInTemp.x0 = 124;
  s_structInTemp.y0 = 366;
  s_structInTemp.width = 60;
  s_structInTemp.background = (u32)s_arrInTempBackgound;
  CreateTextWidget(&s_structInTemp);
  sprintf(string, "--");
  s_structInTemp.setText(&s_structInTemp, string);

  //�ⲿ�¶�
  InitTextWidgetStruct(&s_structExTemp);
  s_structExTemp.x0 = 394;
  s_structExTemp.y0 = 366;
  s_structExTemp.width = 60;
  s_structExTemp.background = (u32)s_arrExTempBackgound;
  CreateTextWidget(&s_structExTemp);
  sprintf(string, "--");
  s_structExTemp.setText(&s_structExTemp, string);

  //�ⲿʪ��
  InitTextWidgetStruct(&s_structHumidity);
  s_structHumidity.x0 = 665;
  s_structHumidity.y0 = 366;
  s_structHumidity.width = 60;
  s_structHumidity.background = (u32)s_arrHumidityBackgound;
  CreateTextWidget(&s_structHumidity);
  sprintf(string, "--");
  s_structHumidity.setText(&s_structHumidity, string);

  //��ʼ��ADC
  InitADC();

  //��ʼ��SHT20
  InitSHT20();
}

/*********************************************************************************************************
* �������ƣ�DeleteGUITemp
* �������ܣ�ɾ����ʪ����ʾ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUITemp(void)
{

}

/*********************************************************************************************************
* �������ƣ�GUITempPoll
* �������ܣ���ʪ����ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUITempPoll(void)
{
  static u64 s_iLastTime = 0;
  double inTemp, exTemp, exHumidity;

  if((GUIGetSysTime() - s_iLastTime) > 250)
  {
    s_iLastTime = GUIGetSysTime();

    //��ȡCPU�ڲ��¶Ȳ������
    inTemp = GetInTemp();
    // printf("CPU�ڲ��¶�ֵ��%.1f��\r\n", inTemp);

    //��ȡ�ⲿ��ʪ�Ȳ������
    exTemp = GetSHT20Temp();
    exHumidity = GetSHT20RH();
    // printf("�ⲿ��ʪ�ȣ�%.1f��, %0.f%%RH\r\n", exTemp, exHumidity);

    //������ʪ����ʾ
    UpdataInTempShow(inTemp);
    UpdataExTempShow(exTemp);
    UpdateExHumidityShow(exHumidity);
  }
}
