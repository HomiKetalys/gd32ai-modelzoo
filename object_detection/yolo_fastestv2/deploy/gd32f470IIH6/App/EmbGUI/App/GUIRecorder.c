/*********************************************************************************************************
* 模块名称：GUIRecorder.c
* 摘    要：录音模块
* 当前版本：1.0.0
* 作    者：Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* 完成日期：2021年07月01日
* 内    容：
* 注    意：
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
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
*                                              宏定义
*********************************************************************************************************/
#define TEXT_Y0       (605) //名字显示纵坐标起点
#define PROGRESS_Y0   (650) //进度条起始纵坐标
#define PROGRESS_X0   (80)  //进度条起始横坐标
#define PROGRESS_X1   (400) //进度条终止横坐标
#define PROGRESS_SIZE (6)   //进度条线条宽度（必须是双数）

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
//按键枚举
typedef enum
{
  GUI_BUTTON_PLAY = 0, //播放按键，必须是从0
  GUI_BUTTON_RECORD,   //录音按键
  GUI_BUTTON_PAUSE,    //暂停按键
  GUI_BUTTON_MAX,      //按键数量
  GUI_BUTTON_NONE,     //无效按键
}EnumGUIButtons;

//喇叭状态
typedef enum
{
  SPK_STATE_OFF,
  SPK_STATE_ON,
}EnumSPKState;

//喇叭状态
static EnumSPKState s_enumSPKState = SPK_STATE_OFF;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX];  //触摸按键
static u16*               s_pTextBackground = NULL;     //字符串显示背景
static u16*               s_pProgressBackground = NULL; //进度条背景

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void UpdataText(char* text);                       //更新名字/时间显示
static void ClearProgress(void);                          //清除进度条显示
static void BeginShowProgress(void);                      //开始显示进度条
static void UpdataProgress(u32 currentTime, u32 allTime); //更新进度条显示
static void ShowRecordTime(void);                         //显示录音时长
static void PlayCallback(void* button);                   //播放按键回调函数
static void RecordeCallback(void* button);                //录音按键回调函数
static void StopCallback(void* button);                   //暂停按键回调函数
static void SPKSwitch(void);                              //切换喇叭状态

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：UpdataText
* 函数功能：更新名字/时间显示
* 输入参数：text：字符串
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void UpdataText(char* text)
{
  u32 len, max, total, begin, width;

  //计算名字长度
  len = 0;
  while((0 != text[len]) && ('\r' != text[len]) && ('\n' != text[len]))
  {
    len++;
  }

  //获取LCD宽度
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];

  //计算屏幕一行最多显示多少个字符
  max = width / 12;

  //所需的像素点总数
  total = 12 * len;

  //计算起始显示像素点
  begin = (width - total) / 2;

  //绘制背景图片
  if(NULL != s_pTextBackground)
  {
    GUIDrawBackground(0, TEXT_Y0, width, 24, (u32)s_pTextBackground);
  }

  //显示名字
  GUIDrawTextLine(begin, TEXT_Y0, (u32)text, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1, max);
}

/*********************************************************************************************************
* 函数名称：ClearProgress
* 函数功能：清除进度条显示
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
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
* 函数名称：BeginShowProgress
* 函数功能：开始显示进度条
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void BeginShowProgress(void)
{
  GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, PROGRESS_X1, PROGRESS_Y0, PROGRESS_SIZE, GUI_COLOR_WHITE, GUI_LINE_CIRCLE);
}

/*********************************************************************************************************
* 函数名称：UpdataSongProgress
* 函数功能：更新进度条显示
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：每隔100ms才会更新一次进度条显示
*********************************************************************************************************/
static void UpdataProgress(u32 currentTime, u32 allTime)
{
  static u64 s_iLastTime    = 0; //上一次更新的时间（ms）
  static u64 s_iCurrentTime = 0; //当前时间（ms）
  u32 endPos;

  s_iCurrentTime = GetSysTime();
  if((s_iCurrentTime - s_iLastTime) > 100)
  {
    //保存系统时间
    s_iLastTime = s_iCurrentTime;

    //计算进度
    endPos = PROGRESS_X0 + (PROGRESS_X1 - PROGRESS_X0) * currentTime / allTime;

    //显示进度
    GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, endPos, PROGRESS_Y0, PROGRESS_SIZE - 1, GUI_COLOR_ORANGE, GUI_LINE_CIRCLE);
  }
}

/*********************************************************************************************************
* 函数名称：ShowRecordTime
* 函数功能：显示录音时长
* 输入参数：size：采集到的音频数据总量
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、为避免频繁更新显示造成卡顿，每隔500ms更新一次时间显示
*          2、采样率为8khz下1s能采集8k个点，每个点2字节，相当于每秒16k字节,据此可以推算出录音时长
*********************************************************************************************************/
static void ShowRecordTime(void)
{
  static char s_iTimeBuf[64];    //显示时间字符串转换缓冲区
  static u64  s_ilastTime   = 0; //上次更新显示系统时间
  static u64  s_iCurrenTime = 0; //当前系统时间

  u32 allTime, secound, min, hour;

  //获取当前时间
  s_iCurrenTime = GetSysTime();
  if((s_iCurrenTime - s_ilastTime) >= 500)
  {
    //计算时钟总长(s)
    allTime = GetRecordeTime();

    //计算秒
    secound = allTime % 60;

    //计算分钟
    allTime = allTime / 60;
    min = allTime % 60;

    //计算小时
    allTime = allTime / 60;
    hour = allTime;

    //字符串转换
    sprintf((char*)s_iTimeBuf, "%02d:%02d:%02d", hour, min, secound);

    //刷新显示
    UpdataText((char*)s_iTimeBuf);

    //保存时间
    s_ilastTime = s_iCurrenTime;
  }
}

/*********************************************************************************************************
* 函数名称：PlayCallback
* 函数功能：播放按键回调函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void PlayCallback(void* button)
{
  //录音机和Wav播放器都处于空闲状态下才能播放录音文件
  if(IsRecorderIdle())
  {
    if(IsWavPlayerIdle())
    {
      //显示录音文件名字
      UpdataText(GetRecordeName());

      //开启进度条显示
      BeginShowProgress();

      //初始化Wav播放器
      InitWavPlayer();

      //设置耳机音量
      WM8978HPvolSet(31, 31);

      //设置喇叭音量
      if(SPK_STATE_ON == s_enumSPKState)
      {
        WM8978SPKvolSet(31);
      }
      else
      {
        WM8978SPKvolSet(0);
      }

      //设置播放歌曲
      SetWavFileName(GetRecordeName());

      //开始播放录音
      StartWavPlay();
    }
  }
}

/*********************************************************************************************************
* 函数名称：RecordeCallback
* 函数功能：录音按键回调函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void RecordeCallback(void* button)
{
  //当前未在播放歌曲
  if(IsWavPlayerIdle())
  {
    //开启录音预处理
    if(IsRecorderIdle())
    {
      //初始化录音机
      InitRecorder();

      //删除进度条
      ClearProgress();
    }
    else
    {
      //显示录音文件名
      UpdataText(GetRecordeName());
    }

    StartEndRecorder();
  }
}

/*********************************************************************************************************
* 函数名称：StopCallback
* 函数功能：暂停按键回调函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void StopCallback(void* button)
{
  //暂停/继续录音
  if(!IsRecorderIdle())
  {
    PauseContinueRecoder();
  }

  //Wav播放
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
* 函数名称：SPKSwitch
* 函数功能：切换喇叭状态
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
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
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUIRecorder
* 函数功能：创建录音模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIRecorder(void)
{
  u32 width;

  //LCD竖屏显示
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //绘制背景
  DisPlayBMPByIPA(0, 0, RECORDER_BACKGROUND_DIR);

  //创建播放按钮
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

  //创建录音按钮
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

  //创建暂停按钮
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

  //获取LCD宽度
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];

  //为歌名/时间显示区域背景图片申请内存
  s_pTextBackground = MyMalloc(SRAMEX, width * 24 * 2);
  if(NULL == s_pTextBackground)
  {
    printf("CreateGUIRecorder: Fail to malloc for text\r\n");
    while(1){}
  }

  //保存歌名/时间显示区域背景
  GUISaveBackground(0, TEXT_Y0, width, 24, (u32)s_pTextBackground);

  //为进度条背景图片申请内存
  s_pProgressBackground = MyMalloc(SRAMEX, width * (PROGRESS_SIZE + 2) * 2);
  if(NULL == s_pProgressBackground)
  {
    printf("CreateGUIRecorder: Fail to malloc for progress bar\r\n");
    while(1){}
  }

  //保存进度条背景
  GUISaveBackground(0, PROGRESS_Y0 - PROGRESS_SIZE / 2, width, PROGRESS_SIZE + 2, (u32)s_pProgressBackground);

  //初始化录音机
  InitRecorder();

  //初始化Wav播放器
  InitWavPlayer();
}

/*********************************************************************************************************
* 函数名称：DeleteGUIRecorder
* 函数功能：删除录音模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
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
* 函数名称：GUIRecorderPoll
* 函数功能：录音模块轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIRecorderPoll(void)
{
  u32 i, playTime, songTime;

  //按键扫描
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    ScanButtonWidget(&s_arrButton[i]);
  }

  //录音模式下时长显示
  if(!IsRecorderIdle())
  {
    ShowRecordTime();
  }

  //Wav播放模式下更新进度条显示
  if(!IsWavPlayerIdle())
  {
    GetWavTime(&playTime, &songTime);
    UpdataProgress(playTime, songTime);
  }

  //Wav播放器轮询
  WavPlayerPoll();

  //录音轮询
  RecorderPoll();

  //独立按键扫描，按下任意按键切换喇叭状态
  ScanKeyOne(KEY_NAME_KEY1, NULL, SPKSwitch);
  ScanKeyOne(KEY_NAME_KEY2, NULL, SPKSwitch);
  ScanKeyOne(KEY_NAME_KEY3, NULL, SPKSwitch);
}
