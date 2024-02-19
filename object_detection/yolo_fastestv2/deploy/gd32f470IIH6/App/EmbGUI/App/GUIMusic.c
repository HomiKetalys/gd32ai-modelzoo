/*********************************************************************************************************
* ģ�����ƣ�GUIMusic.c
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
*                                              �궨��
*********************************************************************************************************/
#define MAX_SONG_NAME_LEN (127) //����������޶�
#define NEME_Y0           (625) //������ʾ���������
#define PROGRESS_Y0       (670)
#define PROGRESS_X0       (80)
#define PROGRESS_X1       (400)

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
//����ö��
typedef enum
{
  GUI_BUTTON_PREVIOUS = 0, //��һ�����������������Ǵ�0
  GUI_BUTTON_NEXT,         //��һ������
  GUI_BUTTON_PLAY_PAUSE,   //����/��ͣ��ť
  GUI_BUTTON_LOW_VOLUME,   //��С����
  GUI_BUTTON_LARGE_VOLUME, //��������
  GUI_BUTTON_MAX,          //��������
  GUI_BUTTON_NONE,         //��Ч����
}EnumGUIButtons;

//����״̬
typedef enum
{
  SPK_STATE_OFF,
  SPK_STATE_ON,
}EnumSPKState;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX];  //��������
static u16*               s_arrButtonBackground = NULL; //���š���ͣ��������
static u16*               s_pNameBaground       = NULL; //������ʾ����

//ͼƬ���
static StructBmpImage s_structPlayImage;
static StructBmpImage s_structPauseImage;

//������ر���
static u32  s_iVolume      = 30; //��������Χ0-63
static u32  s_iNumOfSongs  = 0;  //Ŀ¼�¸�������
static u32  s_iCurrentSong = 0;  //��ǰ����λ��
static u32  s_iNameOffset  = 0;  //��ǰ�������ֻ������и�������������׵�ַ��ƫ����
static char s_arrCurrentSongName[MAX_SONG_NAME_LEN + 1]; //��ǰ�������֣���·����ĩβ�̶�Ϊ0��

//����״̬
static EnumSPKState s_enumSPKState = SPK_STATE_OFF;

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void GetCurrentSongName(void);                         //��ȡ��ǰ��������
static void VolumeUp(void* button);                           //��������
static void VolumeDown(void* button);                         //��С����
static void NextSone(void* button);                           //��һ��
static void PreviousSone(void* button);                       //��һ��
static void PlayPause(void* button);                          //���š���ͣ
static void UpdataSongName(char* songName);                   //���¸�������ʾ
static void UpdataSongProgress(u32 currentTime, u32 allTime); //���½�������ʾ
static void SPKSwitch(void);                                  //�л�����״̬

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�GetCurrentSongName
* �������ܣ���ȡ��ǰ�������ֲ����浽s_arrCurrentSongName
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��ȡ���ǰ���·�������ֵ������ļ����������浽s_arrCurrentSongName
*********************************************************************************************************/
static void GetCurrentSongName(void)
{
  static const char* s_pDir = "0:/music"; //����·��
  FRESULT result;   //�ļ��������ر���
  DIR     direct;   //·��
  FILINFO fileInfo; //�ļ���Ϣ
  u32     songCnt;  //��������
  u32     i;        //ѭ������

  //���ļ����ڴ�����
  #if (1 == _USE_LFN)
    fileInfo.lfname = MyMalloc(SRAMIN, _MAX_LFN);
    if(NULL == fileInfo.lfname)
    {
      printf("���ļ��������ڴ�ʧ��!!!\r\n");
      while(1);
    }
    else
    {
      fileInfo.lfsize = _MAX_LFN;
    }
  #endif

  //��յ�ǰ��������
  for(i = 0; i <= MAX_SONG_NAME_LEN; i++)
  {
    s_arrCurrentSongName[i] = 0;
  }

  //Ŀ¼�¸�������Ϊ0
  if(0 == s_iNumOfSongs)
  {
    return;
  }

  //����·�������ֻ�����
  i = 0;
  while((0 != s_pDir[i]) && (i < MAX_SONG_NAME_LEN - 1))
  {
    s_arrCurrentSongName[i] = s_pDir[i];
    i++;
  }
  s_arrCurrentSongName[i] = '/';
  s_iNameOffset = i + 1;

  //���ض�·��
  result = f_opendir(&direct, s_pDir);
  if(result != FR_OK)
  {
    printf("·��������\r\n");
  }

  //����ָ��λ�ø�������ȡ����
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
      //У���Ƿ�Ϊ�����ļ�
      if(1 == FileSuffixCheck(fileInfo.fname, "Wav"))
      {
        //�ҵ���Ŀ�����
        if(songCnt == s_iCurrentSong)
        {
          //�����������ֵ�������
          i = 0;

          //���ļ���
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

          //���ļ���
          #else
            while((0 != fileInfo.fname[i]) && ((i + s_iNameOffset) < MAX_SONG_NAME_LEN))
            {
              s_arrCurrentSongName[i + s_iNameOffset] = fileInfo.fname[i];
              i++;
            }
          #endif

          //�˳�ѭ��
          break;
        }

        //��û��ָ��λ��
        else
        {
          songCnt++;
        }
      }
    }
  }

  //�ر�Ŀ¼
  result = f_closedir(&direct);
  if(result != FR_OK)
  {
    printf("�ر�Ŀ¼ʧ��\r\n");
  }

  //���ļ����ڴ��ͷ�
  #if (1 == _USE_LFN)
    MyFree(SRAMIN, fileInfo.lfname);
  #endif
}

/*********************************************************************************************************
* �������ƣ�VolumeUp
* �������ܣ���������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�VolumeDown
* �������ܣ���С����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�NextSone
* �������ܣ���һ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void NextSone(void* button)
{
  //���¸���λ��
  s_iCurrentSong = (s_iCurrentSong + 1) % s_iNumOfSongs;

  //���µ�ǰ��������
  GetCurrentSongName();

  //���ò��Ÿ���
  SetWavFileName(s_arrCurrentSongName);

  //��ʾ��������
  UpdataSongName(s_arrCurrentSongName);

  //��������
  RestartWavPlay();
}

/*********************************************************************************************************
* �������ƣ�PreviousSone
* �������ܣ���һ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void PreviousSone(void* button)
{
  //���¸���λ��
  if(0 != s_iCurrentSong)
  {
    s_iCurrentSong = s_iCurrentSong - 1;
  }
  else
  {
    s_iCurrentSong = s_iNumOfSongs - 1;
  }

  //���µ�ǰ��������
  GetCurrentSongName();

  //���ò��Ÿ���
  SetWavFileName(s_arrCurrentSongName);

  //��ʾ��������
  UpdataSongName(s_arrCurrentSongName);

  //��������
  RestartWavPlay();
}

/*********************************************************************************************************
* �������ƣ�PlayPause
* �������ܣ����š���ͣ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void PlayPause(void* button)
{
  EnumWavPlayerState state;
  state = GetWavPlayerState();

  //����״̬�²��ŵ�ǰ����
  if(WAV_PLAYER_STATE_IDLE == state)
  {
    //���ò��Ÿ���
    SetWavFileName(s_arrCurrentSongName);

    //��ʾ��������
    UpdataSongName(s_arrCurrentSongName);

    //��������
    StartWavPlay();
  }

  //�л�����ͣ״̬
  else if(WAV_PLAYER_STATE_PLAY == state)
  {
    PauseWavPlay();
  }

  //��������
  else if(WAV_PLAYER_STATE_PAUSE == state)
  {
    ContinueWavPlay();
  }
}

/*********************************************************************************************************
* �������ƣ�UpdataSongName
* �������ܣ����¸�������ʾ
* ���������songName����������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void UpdataSongName(char* songName)
{
  char filename[MAX_SONG_NAME_LEN];
  u32 i, pos, len, max, total, begin, width;
  char* name;

  //������������
  i = 0;
  while((0 != songName[i]) && ('\r' != songName[i]) && ('\n' != songName[i]))
  {
    filename[i] = songName[i];
    i++;
  }
  filename[i] = 0;
  name = filename;

  //��ѯ���һ��'/'����λ��
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

  //ɾ��·����Ϣ
  if(0xFFFFFFFF != pos)
  {
    name = name + pos + 1;
  }

  //��ѯ���һ��'.'����λ��
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

  //ɾ����׺��Ϣ
  if(0xFFFFFFFF != pos)
  {
    name[pos] = 0;
  }

  //��ȡ���ڿ��
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];

  //�������ֳ���
  len = 0;
  while(0 != name[len])
  {
    len++;
  }

  //������Ļһ�������ʾ���ٸ��ַ�
  max = width / 12;

  //��������ص�����
  total = 12 * len;

  //������ʼ��ʾ���ص�
  begin = (width - total) / 2;

  //���Ʊ���ͼƬ
  if(NULL != s_pNameBaground)
  {
    GUIDrawBackground(0, NEME_Y0, width, 24, (u32)s_pNameBaground);
  }

  //��ʾ����
  GUIDrawTextLine(begin, NEME_Y0, (u32)name, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1, max);
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
static void UpdataSongProgress(u32 currentTime, u32 allTime)
{
  static u64 s_iLastTime    = 0; //��һ�θ��µ�ʱ�䣨ms��
  static u64 s_iCurrentTime = 0; //��ǰʱ�䣨ms��
  static u32 s_iLastendPos  = 0; //�ϴν��ȣ������ж��Ƿ���Ҫ��ս�����
  u32 endPos;

  s_iCurrentTime = GUIGetSysTime();
  if((s_iCurrentTime - s_iLastTime) > 100)
  {
    //����ϵͳʱ��
    s_iLastTime = s_iCurrentTime;

    //�������
    endPos = PROGRESS_X0 + (PROGRESS_X1 - PROGRESS_X0) * currentTime / allTime;

    //��ʾ��������ɫ����
    if(endPos < s_iLastendPos)
    {
      GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, PROGRESS_X1, PROGRESS_Y0, 6, GUI_COLOR_WHITE, GUI_LINE_CIRCLE);
    }

    //��ʾ����
    GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, endPos, PROGRESS_Y0, 6, GUI_COLOR_ORANGE, GUI_LINE_CIRCLE);

    //�������
    s_iLastendPos = endPos;
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
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUIMusic
* �������ܣ���������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateGUIMusic(void)
{
  FRESULT result;   //�ļ��������ر���
  DIR     direct;   //·��
  FILINFO fileInfo; //�ļ���Ϣ

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //���Ʊ���
  DisPlayBMPByIPA(0, 0, MUSIC_BACKGROUND_DIR);

  //Ϊ���š���ͣ�������������ڴ�
  s_arrButtonBackground = MyMalloc(SRAMEX, 50 * 50 * 2);
  if(NULL == s_arrButtonBackground)
  {
    printf("CreateGUIMusic: Fail to malloc\r\n");
    while(1){}
  }

  //Ϊ����������ʾ���򱳾�ͼƬ�����ڴ�
  s_pNameBaground = MyMalloc(SRAMEX, g_structTLILCDDev.width[g_structTLILCDDev.currentLayer] * 24 * 2);
  if(NULL == s_pNameBaground)
  {
    printf("CreateGUIMusic: Fail to malloc\r\n");
    while(1){}
  }
  else
  {
    //���汳��
    GUISaveBackground(0, NEME_Y0, g_structTLILCDDev.width[g_structTLILCDDev.currentLayer], 24, (u32)s_pNameBaground);
  }

  //λͼ��ʼ��
  s_structPlayImage.addr = (void*)MUSIC_PLAY_DIR;
  s_structPlayImage.alphaType = BMP_UES_ALPHA;
  s_structPlayImage.storageType = BMP_IN_FATFS;
  s_structPauseImage.addr = (void*)MUSIC_PAUSE_DIR;
  s_structPauseImage.alphaType = BMP_UES_ALPHA;
  s_structPauseImage.storageType = BMP_IN_FATFS;

  //��С������ť
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

  //������һ����ť
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

  //��������/��ͣ��ť
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

  //������һ����ť
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

  //������������
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

  //���ļ����ڴ�����
  #if (1 == _USE_LFN)
    fileInfo.lfname = MyMalloc(SRAMIN, _MAX_LFN);
    if(NULL == fileInfo.lfname)
    {
      printf("���ļ��������ڴ�ʧ��!!!\r\n");
      while(1);
    }
    else
    {
      fileInfo.lfsize = _MAX_LFN;
    }
  #endif

  //���ض�·��
  result = f_opendir(&direct, "0:/music");
  if(result != FR_OK)
  {
    printf("·��������\r\n");
  }
  else
  {
    printf("�ɹ���ָ��Ŀ¼\r\n");
  }

  //��ѯĿ¼�������ļ��������ļ����ִ�ӡ���������
  s_iNumOfSongs = 0;
  while(1)
  {
    result = f_readdir(&direct, &fileInfo);

    //���ļ���
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

          //У���Ƿ�Ϊ�����ļ�
          if(1 == FileSuffixCheck(fileInfo.lfname, "Wav"))
          {
            s_iNumOfSongs++;
          }
        }
        else
        {
          printf("*/%s\r\n", fileInfo.fname);

          //У���Ƿ�Ϊ�����ļ�
          if(1 == FileSuffixCheck(fileInfo.fname, "Wav"))
          {
            s_iNumOfSongs++;
          }
        }
      }

    //���ļ���
    #else
      if((result != FR_OK) || (0 == fileInfo.fname[0]))
      {
        break;
      }
      else
      {
        printf("*/%s\r\n", fileInfo.fname);

        //У���Ƿ�Ϊ�����ļ�
        if(1 == FileSuffixCheck(fileInfo.fname, "Wav"))
        {
          s_iNumOfSongs++;
        }
      }
    #endif
  }

  //�ر�Ŀ¼
  result = f_closedir(&direct);
  if(result != FR_OK)
  {
    printf("�ر�Ŀ¼ʧ��\r\n");
  }

  //���ļ����ڴ��ͷ�
  #if (1 == _USE_LFN)
    MyFree(SRAMIN, fileInfo.lfname);
  #endif

  //��ӡ��������ļ�����
  printf("����������%d\r\n", s_iNumOfSongs);

  //Ĭ�ϲ��ŵ�һ�׸�
  s_iCurrentSong = 0;

  //���µ�ǰ��������
  GetCurrentSongName();

  //��ʼ��Wav������
  InitWavPlayer();

  //���ö�������
  WM8978HPvolSet(s_iVolume, s_iVolume);

  //�����������
  if(SPK_STATE_OFF == s_enumSPKState)
  {
    WM8978SPKvolSet(0);
  }
  else
  {
    WM8978SPKvolSet(s_iVolume);
  }

  //���ò��Ÿ���
  SetWavFileName(s_arrCurrentSongName);

  //��ʾ��������
  UpdataSongName(s_arrCurrentSongName);

  //��������ʾ
  GUIDrawLine(PROGRESS_X0, PROGRESS_Y0, PROGRESS_X1, PROGRESS_Y0, 6, GUI_COLOR_WHITE, GUI_LINE_CIRCLE);
}

/*********************************************************************************************************
* �������ƣ�DeleteGUIMusic
* �������ܣ�ɾ������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUIMusic(void)
{
  u32 i;

  //ֹͣ���ֲ���
  EndWavPlayer();

  //�ͷ��ڴ�
  MyFree(SRAMEX, s_arrButtonBackground);
  MyFree(SRAMEX, s_pNameBaground);

  //ɾ������
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    DeleteButtonWidget(&s_arrButton[i]);
  }
}

/*********************************************************************************************************
* �������ƣ�GUIMusicPoll
* �������ܣ�������ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIMusicPoll(void)
{
  u32 i, playTime, songTime;
  EnumWavPlayerState state;

  //����ɨ��
  for(i = 0; i < GUI_BUTTON_MAX; i++)
  {
    ScanButtonWidget(&s_arrButton[i]);
  }

  //Wav��������ѯ����
  WavPlayerPoll();

  //��ȡWav������״̬���������ֲ�������ť��ʾ
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

  //��������ʾ
  if((WAV_PLAYER_STATE_PAUSE == state) || (WAV_PLAYER_STATE_PLAY == state))
  {
    GetWavTime(&playTime, &songTime);
    UpdataSongProgress(playTime, songTime);
  }

  //��������ɨ�裬�������ⰴ���л�����״̬
  ScanKeyOne(KEY_NAME_KEY1, NULL, SPKSwitch);
  ScanKeyOne(KEY_NAME_KEY2, NULL, SPKSwitch);
  ScanKeyOne(KEY_NAME_KEY3, NULL, SPKSwitch);
}
