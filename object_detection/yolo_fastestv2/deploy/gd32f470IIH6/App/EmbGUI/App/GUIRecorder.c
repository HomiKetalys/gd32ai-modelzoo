/*********************************************************************************************************
* ģ�����ƣ�GUIRecorder.c
* ժ    Ҫ��¼��ģ��
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
#include "GUIRecorder.h"
#include "GUIPlatform.h"
#include "ButtonWidget.h"
#include "TLILCD.h"
#include "BMP.h"
#include "Common.h"
#include "stdio.h"
#include "Recorder.h"
#include "Malloc.h"
#include "WavPlayer.h"
#include "Timer.h"
#include "WM8978.h"
#include "KeyOne.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define TEXT_Y0       (605) //������ʾ���������
#define PROGRESS_Y0   (650) //��������ʼ������
#define PROGRESS_X0   (80)  //��������ʼ������
#define PROGRESS_X1   (400) //��������ֹ������
#define PROGRESS_SIZE (6)   //������������ȣ�������˫����

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
//����ö��
typedef enum
{
  GUI_BUTTON_PLAY = 0, //���Ű����������Ǵ�0
  GUI_BUTTON_RECORD,   //¼������
  GUI_BUTTON_PAUSE,    //��ͣ����
  GUI_BUTTON_MAX,      //��������
  GUI_BUTTON_NONE,     //��Ч����
}EnumGUIButtons;

//����״̬
typedef enum
{
  SPK_STATE_OFF,
  SPK_STATE_ON,
}EnumSPKState;

//����״̬
static EnumSPKState s_enumSPKState = SPK_STATE_OFF;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX];  //��������
static u16*               s_pTextBackground = NULL;     //�ַ�����ʾ����
static u16*               s_pProgressBackground = NULL; //����������

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void UpdataText(char* text);                       //��������/ʱ����ʾ
static void ClearProgress(void);                          //�����������ʾ
static void BeginShowProgress(void);                      //��ʼ��ʾ������
static void UpdataProgress(u32 currentTime, u32 allTime); //���½�������ʾ
static void ShowRecordTime(void);                         //��ʾ¼��ʱ��
static void PlayCallback(void* button);                   //���Ű����ص�����
static void RecordeCallback(void* button);                //¼�������ص�����
static void StopCallback(void* button);                   //��ͣ�����ص�����
static void SPKSwitch(void);                              //�л�����״̬

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�UpdataText
* �������ܣ���������/ʱ����ʾ
* ���������text���ַ���
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void UpdataText(char* text)
{
  u32 len, max, total, begin, width;

  //�������ֳ���
  len = 0;
  while((0 != text[len]) && ('\r' != text[len]) && ('\n' != text[len]))
  {
    len++;
  }

  //��ȡLCD���
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];

  //������Ļһ�������ʾ���ٸ��ַ�
  max = width / 12;

  //��������ص�����
  total = 12 * len;

  //������ʼ��ʾ���ص�
  begin = (width - total) / 2;

  //���Ʊ���ͼƬ
  if(NULL != s_pTextBackground)
  {
    GUIDrawBackground(0, TEXT_Y0, width, 24, (u32)s_pTextBackground);
  }

  //��ʾ����
  GUIDrawTextLine(begin, TEXT_Y0, (u32)text, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1, max);
}

/*********************************************************************************************************
* �������ƣ�ClearProgress
* �������ܣ������������ʾ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ClearProgress(void)
{
  u32 width;

  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
  if(NULL != s_pProgressBackground)
  {
    GUIDrawBackground(0, PROGRESS_Y0 - PROGRESS_SIZE / 2, width, PROGRESS_SIZE + 2, (u32)s_pProgressBackground);
  }
}

/*********************************************************************************************************
* �������ƣ�BeginShowProgress
* �������ܣ���ʼ��ʾ������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void BeginShowProgress(void)
{
  GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, PROGRESS_X1, PROGRESS_Y0, PROGRESS_SIZE, GUI_COLOR_WHITE, GUI_LINE_CIRCLE);
}

/*********************************************************************************************************
* �������ƣ�UpdataSongProgress
* �������ܣ����½�������ʾ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺ÿ��100ms�Ż����һ�ν�������ʾ
*********************************************************************************************************/
static void UpdataProgress(u32 currentTime, u32 allTime)
{
  static u64 s_iLastTime    = 0; //��һ�θ��µ�ʱ�䣨ms��
  static u64 s_iCurrentTime = 0; //��ǰʱ�䣨ms��
  u32 endPos;

  s_iCurrentTime = GetSysTime();
  if((s_iCurrentTime - s_iLastTime) > 100)
  {
    //����ϵͳʱ��
    s_iLastTime = s_iCurrentTime;

    //�������
    endPos = PROGRESS_X0 + (PROGRESS_X1 - PROGRESS_X0) * currentTime / allTime;

    //��ʾ����
    GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, endPos, PROGRESS_Y0, PROGRESS_SIZE - 1, GUI_COLOR_ORANGE, GUI_LINE_CIRCLE);
  }
}

/*********************************************************************************************************
* �������ƣ�ShowRecordTime
* �������ܣ���ʾ¼��ʱ��
* ���������size���ɼ�������Ƶ��������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��Ϊ����Ƶ��������ʾ��ɿ��٣�ÿ��500ms����һ��ʱ����ʾ
*          2��������Ϊ8khz��1s�ܲɼ�8k���㣬ÿ����2�ֽڣ��൱��ÿ��16k�ֽ�,�ݴ˿��������¼��ʱ��
*********************************************************************************************************/
static void ShowRecordTime(void)
{
  static char s_iTimeBuf[64];    //��ʾʱ���ַ���ת��������
  static u64  s_ilastTime   = 0; //�ϴθ�����ʾϵͳʱ��
  static u64  s_iCurrenTime = 0; //��ǰϵͳʱ��

  u32 allTime, secound, min, hour;

  //��ȡ��ǰʱ��
  s_iCurrenTime = GetSysTime();
  if((s_iCurrenTime - s_ilastTime) >= 500)
  {
    //����ʱ���ܳ�(s)
    allTime = GetRecordeTime();

    //������
    secound = allTime % 60;

    //�������
    allTime = allTime / 60;
    min = allTime % 60;

    //����Сʱ
    allTime = allTime / 60;
    hour = allTime;

    //�ַ���ת��
    sprintf((char*)s_iTimeBuf, "%02d:%02d:%02d", hour, min, secound);

    //ˢ����ʾ
    UpdataText((char*)s_iTimeBuf);

    //����ʱ��
    s_ilastTime = s_iCurrenTime;
  }
}

/*********************************************************************************************************
* �������ƣ�PlayCallback
* �������ܣ����Ű����ص�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void PlayCallback(void* button)
{
  //¼������Wav�����������ڿ���״̬�²��ܲ���¼���ļ�
  if(IsRecorderIdle())
  {
    if(IsWavPlayerIdle())
    {
      //��ʾ¼���ļ�����
      UpdataText(GetRecordeName());

      //������������ʾ
      BeginShowProgress();

      //��ʼ��Wav������
      InitWavPlayer();

      //���ö�������
      WM8978HPvolSet(31, 31);

      //������������
      if(SPK_STATE_ON == s_enumSPKState)
      {
        WM8978SPKvolSet(31);
      }
      else
      {
        WM8978SPKvolSet(0);
      }

      //���ò��Ÿ���
      SetWavFileName(GetRecordeName());

      //��ʼ����¼��
      StartWavPlay();
    }
  }
}

/*********************************************************************************************************
* �������ƣ�RecordeCallback
* �������ܣ�¼�������ص�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void RecordeCallback(void* button)
{
  //��ǰδ�ڲ��Ÿ���
  if(IsWavPlayerIdle())
  {
    //����¼��Ԥ����
    if(IsRecorderIdle())
    {
      //��ʼ��¼����
      InitRecorder();

      //ɾ��������
      ClearProgress();
    }
    else
    {
      //��ʾ¼���ļ���
      UpdataText(GetRecordeName());
    }

    StartEndRecorder();
  }
}

/*********************************************************************************************************
* �������ƣ�StopCallback
* �������ܣ���ͣ�����ص�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void StopCallback(void* button)
{
  //��ͣ/����¼��
  if(!IsRecorderIdle())
  {
    PauseContinueRecoder();
  }

  //Wav����
  if(!IsWavPlayerIdle())
  {
    if(WAV_PLAYER_STATE_PLAY == GetWavPlayerState())
    {
      PauseWavPlay();
    }
    else if(WAV_PLAYER_STATE_PAUSE == GetWavPlayerState())
    {
      ContinueWavPlay();
    }
  }
}

/*********************************************************************************************************
* �������ƣ�SPKSwitch
* �������ܣ��л�����״̬
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void SPKSwitch(void)
{
  if(SPK_STATE_OFF == s_enumSPKState)
  {
    s_enumSPKState = SPK_STATE_ON;
    WM8978SPKvolSet(31);
    printf("SPKSwitch: speaker on\r\n");
  }
  else
  {
    s_enumSPKState = SPK_STATE_OFF;
    WM8978SPKvolSet(0);
    printf("SPKSwitch: speaker off\r\n");
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUIRecorder
* �������ܣ�����¼��ģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateGUIRecorder(void)
{
  u32 width;

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //���Ʊ���
  DisPlayBMPByIPA(0, 0, RECORDER_BACKGROUND_DIR);

  //�������Ű�ť
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PLAY]);
  s_arrButton[GUI_BUTTON_PLAY].x0 = 74;
  s_arrButton[GUI_BUTTON_PLAY].y0 = 700;
  s_arrButton[GUI_BUTTON_PLAY].width = 50;
  s_arrButton[GUI_BUTTON_PLAY].height = 50;
  s_arrButton[GUI_BUTTON_PLAY].text = "";
  s_arrButton[GUI_BUTTON_PLAY].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PLAY].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PLAY].pressCallback = PlayCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PLAY]);

  //����¼����ť
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_RECORD]);
  s_arrButton[GUI_BUTTON_RECORD].x0 = 180;
  s_arrButton[GUI_BUTTON_RECORD].y0 = 660;
  s_arrButton[GUI_BUTTON_RECORD].width = 128;
  s_arrButton[GUI_BUTTON_RECORD].height = 128;
  s_arrButton[GUI_BUTTON_RECORD].text = "";
  s_arrButton[GUI_BUTTON_RECORD].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_RECORD].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_RECORD].pressCallback = RecordeCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_RECORD]);

  //������ͣ��ť
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PAUSE]);
  s_arrButton[GUI_BUTTON_PAUSE].x0 = 354;
  s_arrButton[GUI_BUTTON_PAUSE].y0 = 700;
  s_arrButton[GUI_BUTTON_PAUSE].width = 50;
  s_arrButton[GUI_BUTTON_PAUSE].height = 50;
  s_arrButton[GUI_BUTTON_PAUSE].text = "";
  s_arrButton[GUI_BUTTON_PAUSE].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PAUSE].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PAUSE].pressCallback = StopCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PAUSE]);

  //��ȡLCD���
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];

  //Ϊ����/ʱ����ʾ���򱳾�ͼƬ�����ڴ�
  s_pTextBackground = MyMalloc(SRAMEX, width * 24 * 2);
  if(NULL == s_pTextBackground)
  {
    printf("CreateGUIRecorder: Fail to malloc for text\r\n");
    while(1){}
  }

  //�������/ʱ����ʾ���򱳾�
  GUISaveBackground(0, TEXT_Y0, width, 24, (u32)s_pTextBackground);

  //Ϊ����������ͼƬ�����ڴ�
  s_pProgressBackground = MyMalloc(SRAMEX, width * (PROGRESS_SIZE + 2) * 2);
  if(NULL == s_pProgressBackground)
  {
    printf("CreateGUIRecorder: Fail to malloc for progress bar\r\n");
    while(1){}
  }

  //�������������
  GUISaveBackground(0, PROGRESS_Y0 - PROGRESS_SIZE / 2, width, PROGRESS_SIZE + 2, (u32)s_pProgressBackground);

  //��ʼ��¼����
  InitRecorder();

  //��ʼ��Wav������
  InitWavPlayer();
}

/*********************************************************************************************************
* �������ƣ�DeleteGUIRecorder
* �������ܣ�ɾ��¼��ģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUIRecorder(void)
{
  u32 i;
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    DeleteButtonWidget(&s_arrButton[i]);
  }
}

/*********************************************************************************************************
* �������ƣ�GUIRecorderPoll
* �������ܣ�¼��ģ����ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIRecorderPoll(void)
{
  u32 i, playTime, songTime;

  //����ɨ��
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    ScanButtonWidget(&s_arrButton[i]);
  }

  //¼��ģʽ��ʱ����ʾ
  if(!IsRecorderIdle())
  {
    ShowRecordTime();
  }

  //Wav����ģʽ�¸��½�������ʾ
  if(!IsWavPlayerIdle())
  {
    GetWavTime(&playTime, &songTime);
    UpdataProgress(playTime, songTime);
  }

  //Wav��������ѯ
  WavPlayerPoll();

  //¼����ѯ
  RecorderPoll();

  //��������ɨ�裬�������ⰴ���л�����״̬
  ScanKeyOne(KEY_NAME_KEY1, NULL, SPKSwitch);
  ScanKeyOne(KEY_NAME_KEY2, NULL, SPKSwitch);
  ScanKeyOne(KEY_NAME_KEY3, NULL, SPKSwitch);
}
