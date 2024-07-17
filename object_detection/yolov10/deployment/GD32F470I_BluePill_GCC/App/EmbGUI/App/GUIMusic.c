/*********************************************************************************************************
* 模块名称：GUIMusic.c
* 摘    要：音乐模块
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
#include "GUIMusic.h"
#include "GUIPlatform.h"
#include "ButtonWidget.h"
#include "TLILCD.h"
#include "BMP.h"
#include "Common.h"
#include "stdio.h"
#include "Malloc.h"
#include "FontLib.h"
#include "WavPlayer.h"
#include "WM8978.h"
#include "ff.h"
#include "KeyOne.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define MAX_SONG_NAME_LEN (127) //歌曲名字最长限定
#define NEME_Y0           (625) //名字显示纵坐标起点
#define PROGRESS_Y0       (670)
#define PROGRESS_X0       (80)
#define PROGRESS_X1       (400)

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
//按键枚举
typedef enum
{
  GUI_BUTTON_PREVIOUS = 0, //上一曲按键按键，必须是从0
  GUI_BUTTON_NEXT,         //下一曲按键
  GUI_BUTTON_PLAY_PAUSE,   //播放/暂停按钮
  GUI_BUTTON_LOW_VOLUME,   //减小音量
  GUI_BUTTON_LARGE_VOLUME, //增大音量
  GUI_BUTTON_MAX,          //按键数量
  GUI_BUTTON_NONE,         //无效按键
}EnumGUIButtons;

//喇叭状态
typedef enum
{
  SPK_STATE_OFF,
  SPK_STATE_ON,
}EnumSPKState;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX];  //触摸按键
static u16*               s_arrButtonBackground = NULL; //播放、暂停按键背景
static u16*               s_pNameBaground       = NULL; //名字显示背景

//图片相关
static StructBmpImage s_structPlayImage;
static StructBmpImage s_structPauseImage;

//音乐相关变量
static u32  s_iVolume      = 30; //音量，范围0-63
static u32  s_iNumOfSongs  = 0;  //目录下歌曲总数
static u32  s_iCurrentSong = 0;  //当前歌曲位置
static u32  s_iNameOffset  = 0;  //当前歌曲名字缓冲区中歌曲名字相对于首地址的偏移量
static char s_arrCurrentSongName[MAX_SONG_NAME_LEN + 1]; //当前歌曲名字（含路径，末尾固定为0）

//喇叭状态
static EnumSPKState s_enumSPKState = SPK_STATE_OFF;

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void GetCurrentSongName(void);                         //获取当前歌曲名字
static void VolumeUp(void* button);                           //增大音量
static void VolumeDown(void* button);                         //减小音量
static void NextSone(void* button);                           //下一曲
static void PreviousSone(void* button);                       //上一曲
static void PlayPause(void* button);                          //播放、暂停
static void UpdataSongName(char* songName);                   //更新歌曲名显示
static void UpdataSongProgress(u32 currentTime, u32 allTime); //更新进度条显示
static void SPKSwitch(void);                                  //切换喇叭状态

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：GetCurrentSongName
* 函数功能：获取当前歌曲名字并保存到s_arrCurrentSongName
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：获取的是包含路径和名字的完整文件名，并保存到s_arrCurrentSongName
*********************************************************************************************************/
static void GetCurrentSongName(void)
{
  static const char* s_pDir = "0:/music"; //歌曲路径
  FRESULT result;   //文件操作返回变量
  DIR     direct;   //路径
  FILINFO fileInfo; //文件信息
  u32     songCnt;  //歌曲计数
  u32     i;        //循环变量

  //长文件名内存申请
  #if (1 == _USE_LFN)
    fileInfo.lfname = MyMalloc(SRAMIN, _MAX_LFN);
    if(NULL == fileInfo.lfname)
    {
      printf("长文件名申请内存失败!!!\r\n");
      while(1);
    }
    else
    {
      fileInfo.lfsize = _MAX_LFN;
    }
  #endif

  //清空当前歌曲名字
  for(i = 0; i <= MAX_SONG_NAME_LEN; i++)
  {
    s_arrCurrentSongName[i] = 0;
  }

  //目录下歌曲数量为0
  if(0 == s_iNumOfSongs)
  {
    return;
  }

  //拷贝路径到名字缓冲区
  i = 0;
  while((0 != s_pDir[i]) && (i < MAX_SONG_NAME_LEN - 1))
  {
    s_arrCurrentSongName[i] = s_pDir[i];
    i++;
  }
  s_arrCurrentSongName[i] = '/';
  s_iNameOffset = i + 1;

  //打开特定路径
  result = f_opendir(&direct, s_pDir);
  if(result != FR_OK)
  {
    printf("路径不存在\r\n");
  }

  //查找指定位置歌曲并获取名字
  songCnt = 0;
  while(1)
  {
    result = f_readdir(&direct, &fileInfo);
    if((result != FR_OK) || (0 == fileInfo.fname[0]))
    {
      break;
    }
    else
    {
      //校验是否为歌曲文件
      if(1 == FileSuffixCheck(fileInfo.fname, "Wav"))
      {
        //找到了目标歌曲
        if(songCnt == s_iCurrentSong)
        {
          //拷贝歌曲名字到缓冲区
          i = 0;

          //长文件名
          #if _USE_LFN
            if(0 != fileInfo.lfname[0])
            {
              while((0 != fileInfo.lfname[i]) && ((i + s_iNameOffset) < MAX_SONG_NAME_LEN))
              {
                s_arrCurrentSongName[i + s_iNameOffset] = fileInfo.lfname[i];
                i++;
              }
            }
            else
            {
              while((0 != fileInfo.fname[i]) && ((i + s_iNameOffset) < MAX_SONG_NAME_LEN))
              {
                s_arrCurrentSongName[i + s_iNameOffset] = fileInfo.fname[i];
                i++;
              }
            }

          //短文件名
          #else
            while((0 != fileInfo.fname[i]) && ((i + s_iNameOffset) < MAX_SONG_NAME_LEN))
            {
              s_arrCurrentSongName[i + s_iNameOffset] = fileInfo.fname[i];
              i++;
            }
          #endif

          //退出循环
          break;
        }

        //还没到指定位置
        else
        {
          songCnt++;
        }
      }
    }
  }

  //关闭目录
  result = f_closedir(&direct);
  if(result != FR_OK)
  {
    printf("关闭目录失败\r\n");
  }

  //长文件名内存释放
  #if (1 == _USE_LFN)
    MyFree(SRAMIN, fileInfo.lfname);
  #endif
}

/*********************************************************************************************************
* 函数名称：VolumeUp
* 函数功能：增大音量
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void VolumeUp(void* button)
{
  if(s_iVolume <= 58)
  {
    s_iVolume = s_iVolume + 5;
    WM8978HPvolSet(s_iVolume, s_iVolume);
    if(SPK_STATE_ON == s_enumSPKState)
    {
      WM8978SPKvolSet(s_iVolume);
    }
    else
    {
      WM8978SPKvolSet(0);
    }
  }
}

/*********************************************************************************************************
* 函数名称：VolumeDown
* 函数功能：减小音量
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void VolumeDown(void* button)
{
  if(s_iVolume >= 5)
  {
    s_iVolume = s_iVolume - 5;
    WM8978HPvolSet(s_iVolume, s_iVolume);
    if(SPK_STATE_ON == s_enumSPKState)
    {
      WM8978SPKvolSet(s_iVolume);
    }
    else
    {
      WM8978SPKvolSet(0);
    }
  }
}

/*********************************************************************************************************
* 函数名称：NextSone
* 函数功能：下一曲
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void NextSone(void* button)
{
  //更新歌曲位置
  s_iCurrentSong = (s_iCurrentSong + 1) % s_iNumOfSongs;

  //更新当前歌曲名字
  GetCurrentSongName();

  //设置播放歌曲
  SetWavFileName(s_arrCurrentSongName);

  //显示歌曲名字
  UpdataSongName(s_arrCurrentSongName);

  //开启播放
  RestartWavPlay();
}

/*********************************************************************************************************
* 函数名称：PreviousSone
* 函数功能：上一曲
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void PreviousSone(void* button)
{
  //更新歌曲位置
  if(0 != s_iCurrentSong)
  {
    s_iCurrentSong = s_iCurrentSong - 1;
  }
  else
  {
    s_iCurrentSong = s_iNumOfSongs - 1;
  }

  //更新当前歌曲名字
  GetCurrentSongName();

  //设置播放歌曲
  SetWavFileName(s_arrCurrentSongName);

  //显示歌曲名字
  UpdataSongName(s_arrCurrentSongName);

  //开启播放
  RestartWavPlay();
}

/*********************************************************************************************************
* 函数名称：PlayPause
* 函数功能：播放、暂停
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void PlayPause(void* button)
{
  EnumWavPlayerState state;
  state = GetWavPlayerState();

  //空闲状态下播放当前歌曲
  if(WAV_PLAYER_STATE_IDLE == state)
  {
    //设置播放歌曲
    SetWavFileName(s_arrCurrentSongName);

    //显示歌曲名字
    UpdataSongName(s_arrCurrentSongName);

    //开启播放
    StartWavPlay();
  }

  //切换到暂停状态
  else if(WAV_PLAYER_STATE_PLAY == state)
  {
    PauseWavPlay();
  }

  //继续播放
  else if(WAV_PLAYER_STATE_PAUSE == state)
  {
    ContinueWavPlay();
  }
}

/*********************************************************************************************************
* 函数名称：UpdataSongName
* 函数功能：更新歌曲名显示
* 输入参数：songName：歌曲名字
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void UpdataSongName(char* songName)
{
  char filename[MAX_SONG_NAME_LEN];
  u32 i, pos, len, max, total, begin, width;
  char* name;

  //拷贝歌曲名字
  i = 0;
  while((0 != songName[i]) && ('\r' != songName[i]) && ('\n' != songName[i]))
  {
    filename[i] = songName[i];
    i++;
  }
  filename[i] = 0;
  name = filename;

  //查询最后一个'/'所在位置
  i = 0;
  pos = 0xFFFFFFFF;
  while(0 != name[i])
  {
    if('/' == name[i])
    {
      pos = i;
    }
    i++;
  }

  //删除路径信息
  if(0xFFFFFFFF != pos)
  {
    name = name + pos + 1;
  }

  //查询最后一个'.'所在位置
  i = 0;
  pos = 0xFFFFFFFF;
  while(0 != name[i])
  {
    if('.' == name[i])
    {
      pos = i;
    }
    i++;
  }

  //删除后缀信息
  if(0xFFFFFFFF != pos)
  {
    name[pos] = 0;
  }

  //获取窗口宽度
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];

  //计算名字长度
  len = 0;
  while(0 != name[len])
  {
    len++;
  }

  //计算屏幕一行最多显示多少个字符
  max = width / 12;

  //所需的像素点总数
  total = 12 * len;

  //计算起始显示像素点
  begin = (width - total) / 2;

  //绘制背景图片
  if(NULL != s_pNameBaground)
  {
    GUIDrawBackground(0, NEME_Y0, width, 24, (u32)s_pNameBaground);
  }

  //显示名字
  GUIDrawTextLine(begin, NEME_Y0, (u32)name, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1, max);
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
static void UpdataSongProgress(u32 currentTime, u32 allTime)
{
  static u64 s_iLastTime    = 0; //上一次更新的时间（ms）
  static u64 s_iCurrentTime = 0; //当前时间（ms）
  static u32 s_iLastendPos  = 0; //上次进度，用于判断是否需要清空进度条
  u32 endPos;

  s_iCurrentTime = GUIGetSysTime();
  if((s_iCurrentTime - s_iLastTime) > 100)
  {
    //保存系统时间
    s_iLastTime = s_iCurrentTime;

    //计算进度
    endPos = PROGRESS_X0 + (PROGRESS_X1 - PROGRESS_X0) * currentTime / allTime;

    //显示进度条白色背景
    if(endPos < s_iLastendPos)
    {
      GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, PROGRESS_X1, PROGRESS_Y0, 6, GUI_COLOR_WHITE, GUI_LINE_CIRCLE);
    }

    //显示进度
    GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, endPos, PROGRESS_Y0, 6, GUI_COLOR_ORANGE, GUI_LINE_CIRCLE);

    //保存进度
    s_iLastendPos = endPos;
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
    WM8978SPKvolSet(s_iVolume);
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
* 函数名称：CreateGUIMusic
* 函数功能：创建音乐
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIMusic(void)
{
  FRESULT result;   //文件操作返回变量
  DIR     direct;   //路径
  FILINFO fileInfo; //文件信息

  //LCD竖屏显示
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //绘制背景
  DisPlayBMPByIPA(0, 0, MUSIC_BACKGROUND_DIR);

  //为播放、暂停按键背景申请内存
  s_arrButtonBackground = MyMalloc(SRAMEX, 50 * 50 * 2);
  if(NULL == s_arrButtonBackground)
  {
    printf("CreateGUIMusic: Fail to malloc\r\n");
    while(1){}
  }

  //为音乐名字显示区域背景图片申请内存
  s_pNameBaground = MyMalloc(SRAMEX, g_structTLILCDDev.width[g_structTLILCDDev.currentLayer] * 24 * 2);
  if(NULL == s_pNameBaground)
  {
    printf("CreateGUIMusic: Fail to malloc\r\n");
    while(1){}
  }
  else
  {
    //保存背景
    GUISaveBackground(0, NEME_Y0, g_structTLILCDDev.width[g_structTLILCDDev.currentLayer], 24, (u32)s_pNameBaground);
  }

  //位图初始化
  s_structPlayImage.addr = (void*)MUSIC_PLAY_DIR;
  s_structPlayImage.alphaType = BMP_UES_ALPHA;
  s_structPlayImage.storageType = BMP_IN_FATFS;
  s_structPauseImage.addr = (void*)MUSIC_PAUSE_DIR;
  s_structPauseImage.alphaType = BMP_UES_ALPHA;
  s_structPauseImage.storageType = BMP_IN_FATFS;

  //减小音量按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_LOW_VOLUME]);
  s_arrButton[GUI_BUTTON_LOW_VOLUME].x0 = 38;
  s_arrButton[GUI_BUTTON_LOW_VOLUME].y0 = 700;
  s_arrButton[GUI_BUTTON_LOW_VOLUME].width = 50;
  s_arrButton[GUI_BUTTON_LOW_VOLUME].height = 50;
  s_arrButton[GUI_BUTTON_LOW_VOLUME].text = "";
  s_arrButton[GUI_BUTTON_LOW_VOLUME].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_LOW_VOLUME].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_LOW_VOLUME].pressCallback = VolumeDown;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_LOW_VOLUME]);

  //创建上一曲按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PREVIOUS]);
  s_arrButton[GUI_BUTTON_PREVIOUS].x0 = 126;
  s_arrButton[GUI_BUTTON_PREVIOUS].y0 = 700;
  s_arrButton[GUI_BUTTON_PREVIOUS].width = 50;
  s_arrButton[GUI_BUTTON_PREVIOUS].height = 50;
  s_arrButton[GUI_BUTTON_PREVIOUS].text = "";
  s_arrButton[GUI_BUTTON_PREVIOUS].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PREVIOUS].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PREVIOUS].pressCallback = PreviousSone;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PREVIOUS]);

  //创建播放/暂停按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PLAY_PAUSE]);
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].x0 = 214;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].y0 = 702;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].width = 50;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].height = 50;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].text = "";
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].buttonBackground = s_arrButtonBackground;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].pressBackgroudImage = (void*)&s_structPlayImage;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].releaseBackgroudImage = (void*)&s_structPlayImage;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].pressImageType = GUI_IMAGE_TYPE_BMP;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].releaseImageType = GUI_IMAGE_TYPE_BMP;
  s_arrButton[GUI_BUTTON_PLAY_PAUSE].pressCallback = PlayPause;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PLAY_PAUSE]);

  //创建下一曲按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_NEXT]);
  s_arrButton[GUI_BUTTON_NEXT].x0 = 302;
  s_arrButton[GUI_BUTTON_NEXT].y0 = 700;
  s_arrButton[GUI_BUTTON_NEXT].width = 50;
  s_arrButton[GUI_BUTTON_NEXT].height = 50;
  s_arrButton[GUI_BUTTON_NEXT].text = "";
  s_arrButton[GUI_BUTTON_NEXT].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_NEXT].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_NEXT].pressCallback = NextSone;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_NEXT]);

  //创建增加音量
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_LARGE_VOLUME]);
  s_arrButton[GUI_BUTTON_LARGE_VOLUME].x0 = 390;
  s_arrButton[GUI_BUTTON_LARGE_VOLUME].y0 = 700;
  s_arrButton[GUI_BUTTON_LARGE_VOLUME].width = 50;
  s_arrButton[GUI_BUTTON_LARGE_VOLUME].height = 50;
  s_arrButton[GUI_BUTTON_LARGE_VOLUME].text = "";
  s_arrButton[GUI_BUTTON_LARGE_VOLUME].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_LARGE_VOLUME].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_LARGE_VOLUME].pressCallback = VolumeUp;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_LARGE_VOLUME]);

  //长文件名内存申请
  #if (1 == _USE_LFN)
    fileInfo.lfname = MyMalloc(SRAMIN, _MAX_LFN);
    if(NULL == fileInfo.lfname)
    {
      printf("长文件名申请内存失败!!!\r\n");
      while(1);
    }
    else
    {
      fileInfo.lfsize = _MAX_LFN;
    }
  #endif

  //打开特定路径
  result = f_opendir(&direct, "0:/music");
  if(result != FR_OK)
  {
    printf("路径不存在\r\n");
  }
  else
  {
    printf("成功打开指定目录\r\n");
  }

  //查询目录下所有文件，并将文件名字打印输出到串口
  s_iNumOfSongs = 0;
  while(1)
  {
    result = f_readdir(&direct, &fileInfo);

    //长文件名
    #if _USE_LFN
      if((result != FR_OK) || (0 == fileInfo.fname[0]))
      {
        break;
      }
      else
      {
        if(0 != fileInfo.lfname[0])
        {
          printf("*/%s\r\n", fileInfo.lfname);

          //校验是否为歌曲文件
          if(1 == FileSuffixCheck(fileInfo.lfname, "Wav"))
          {
            s_iNumOfSongs++;
          }
        }
        else
        {
          printf("*/%s\r\n", fileInfo.fname);

          //校验是否为歌曲文件
          if(1 == FileSuffixCheck(fileInfo.fname, "Wav"))
          {
            s_iNumOfSongs++;
          }
        }
      }

    //短文件名
    #else
      if((result != FR_OK) || (0 == fileInfo.fname[0]))
      {
        break;
      }
      else
      {
        printf("*/%s\r\n", fileInfo.fname);

        //校验是否为歌曲文件
        if(1 == FileSuffixCheck(fileInfo.fname, "Wav"))
        {
          s_iNumOfSongs++;
        }
      }
    #endif
  }

  //关闭目录
  result = f_closedir(&direct);
  if(result != FR_OK)
  {
    printf("关闭目录失败\r\n");
  }

  //长文件名内存释放
  #if (1 == _USE_LFN)
    MyFree(SRAMIN, fileInfo.lfname);
  #endif

  //打印输出歌曲文件总数
  printf("歌曲数量：%d\r\n", s_iNumOfSongs);

  //默认播放第一首歌
  s_iCurrentSong = 0;

  //更新当前歌曲名字
  GetCurrentSongName();

  //初始化Wav播放器
  InitWavPlayer();

  //设置耳机音量
  WM8978HPvolSet(s_iVolume, s_iVolume);

  //设置喇叭输出
  if(SPK_STATE_OFF == s_enumSPKState)
  {
    WM8978SPKvolSet(0);
  }
  else
  {
    WM8978SPKvolSet(s_iVolume);
  }

  //设置播放歌曲
  SetWavFileName(s_arrCurrentSongName);

  //显示歌曲名字
  UpdataSongName(s_arrCurrentSongName);

  //进度条显示
  GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, PROGRESS_X1, PROGRESS_Y0, 6, GUI_COLOR_WHITE, GUI_LINE_CIRCLE);
}

/*********************************************************************************************************
* 函数名称：DeleteGUIMusic
* 函数功能：删除音乐
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUIMusic(void)
{
  u32 i;

  //停止音乐播放
  EndWavPlayer();

  //释放内存
  MyFree(SRAMEX, s_arrButtonBackground);
  MyFree(SRAMEX, s_pNameBaground);

  //删除按键
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    DeleteButtonWidget(&s_arrButton[i]);
  }
}

/*********************************************************************************************************
* 函数名称：GUIMusicPoll
* 函数功能：音乐轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIMusicPoll(void)
{
  u32 i, playTime, songTime;
  EnumWavPlayerState state;

  //按键扫描
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    ScanButtonWidget(&s_arrButton[i]);
  }

  //Wav播放器轮询任务
  WavPlayerPoll();

  //获取Wav播放器状态并更新音乐播放器按钮显示
  state = GetWavPlayerState();
  if((WAV_PLAYER_STATE_IDLE == state) || (WAV_PLAYER_STATE_PAUSE == state))
  {
    if(&s_structPlayImage != s_arrButton[GUI_BUTTON_PLAY_PAUSE].pressBackgroudImage)
    {
      s_arrButton[GUI_BUTTON_PLAY_PAUSE].pressBackgroudImage = (void*)&s_structPlayImage;
      s_arrButton[GUI_BUTTON_PLAY_PAUSE].releaseBackgroudImage = (void*)&s_structPlayImage;
      UpdateButtonWidget(&s_arrButton[GUI_BUTTON_PLAY_PAUSE]);
    }
  }
  else if(WAV_PLAYER_STATE_PLAY == state)
  {
    if(&s_structPauseImage != s_arrButton[GUI_BUTTON_PLAY_PAUSE].pressBackgroudImage)
    {
      s_arrButton[GUI_BUTTON_PLAY_PAUSE].pressBackgroudImage = (void*)&s_structPauseImage;
      s_arrButton[GUI_BUTTON_PLAY_PAUSE].releaseBackgroudImage = (void*)&s_structPauseImage;
      UpdateButtonWidget(&s_arrButton[GUI_BUTTON_PLAY_PAUSE]);
    }
  }

  //进度条显示
  if((WAV_PLAYER_STATE_PAUSE == state) || (WAV_PLAYER_STATE_PLAY == state))
  {
    GetWavTime(&playTime, &songTime);
    UpdataSongProgress(playTime, songTime);
  }

  //独立按键扫描，按下任意按键切换喇叭状态
  ScanKeyOne(KEY_NAME_KEY1, NULL, SPKSwitch);
  ScanKeyOne(KEY_NAME_KEY2, NULL, SPKSwitch);
  ScanKeyOne(KEY_NAME_KEY3, NULL, SPKSwitch);
}
