/*********************************************************************************************************
* ģ�����ƣ�GUIFileSys.c
* ժ    Ҫ���ļ�ϵͳ��ʾģ��
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
#include "GUIFileSys.h"
#include "TLILCD.h"
#include "GUIPlatform.h"
#include "BMP.h"
#include "ff.h"
#include "stdio.h"
#include "Common.h"
#include "SysTick.h"
#include "Touch.h"
#include "JPEG.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define FILE_INFO_X0      (32)
#define FILE_INFO_X1      (448)
#define FILE_INFO_WIDTH   (FILE_INFO_X1 - FILE_INFO_X0 + 1)

#define DIR_SHOW_Y0       (52)
#define DIR_SHOW_HEIGHT   (34)
#define DIR_FILL_COLOR    ((242 << 16) | (242 << 8) | (242))
#define DIR_FONT          (GUI_FONT_ASCII_24)

#define ICON_WIDTH        (40)     //ͼ����
#define ICON_HEIGHT       (40)     //ͼ��߶�
#define ICON_TEXT_SPACE   (5)      //ͼƬ���ַ�����ʾ�ļ�϶
#define TEXT_TEXT_SPACE   (1)      //���������ַ���֮��ļ�϶
#define BUTTON_LINE_SPACE (2)      //�ײ�������ͼƬ�ļ�϶
#define MAX_NAME_LEN      _MAX_LFN //��ļ���
#define NAME_FONT         (GUI_FONT_ASCII_24)
#define DATE_FONT         (GUI_FONT_ASCII_12)
#define LINE_COLOR        ((217 << 16) | (217 << 8) | (217)) //�׶�������ɫ��RGB888��
#define FILL_COLOR        ((242 << 16) | (242 << 8) | (242)) //ѡ�������ɫ��RGB888��

#define ITEM_SHOW_SPACE   (10)
#define ITEM_SHOW_Y0      (DIR_SHOW_Y0 + DIR_SHOW_HEIGHT + ITEM_SHOW_SPACE)
#define ITEM_HEIGHT       (ICON_HEIGHT + BUTTON_LINE_SPACE)
#define MAX_ITEM_NUM      ((LCD_PIXEL_WIDTH - ITEM_SHOW_Y0) / (ITEM_HEIGHT + ITEM_SHOW_SPACE))

//��������ض���
#define PROBAR_X0         (452)
#define PROBAR_Y0         (140)
#define PROBAR_WIDTH      (15)
#define PROBAR_HEIGHT     (620)
#define PROBAR_X1         (PROBAR_X0 + PROBAR_WIDTH - 1)
#define PROBAR_Y1         (PROBAR_Y0 + PROBAR_HEIGHT - 1)
//#define PROBAR_COLOR      RGB888ToRGB565B((181 << 16) | (210 << 8) | (236 << 0))
#define PROBAR_COLOR      RGB888ToRGB565B((46 << 16) | (117 << 8) | (182 << 0))

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
//�ļ���Ϣ�ṹ��
typedef struct 
{
  u32  x0;       //��ʼ����
  u32  y0;       //��ʼ����
  u32  width;    //���
  u32  height;   //�߶�
  u32  fileType; //�ļ����ͣ�0-��ͨ�ļ���1-·����2-�����ļ�
  u32  dirType;  //0-��Ŀ¼��1-��Ŀ¼
  u32  hasfocus; //���޽��㣬0-�޽��㣬1-�н���
  char fileName[MAX_NAME_LEN]; //�ļ���������·��
  char fileData[MAX_NAME_LEN]; //�ļ���Ϣ
  StructBmpImage image;        //�ļ�ͼ��
}StructFileInfo;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static char    s_arrCurrentDir[MAX_NAME_LEN] = "0:";
static char    s_arrFileLname[MAX_NAME_LEN];
static DIR     s_dirFileDir;   //��ǰ·��
static FILINFO s_infoFileInfo; //��ǰ�ļ���Ϣ

//��Ŀ�ؼ���ÿһ��һ����Ŀ��
static StructFileInfo s_arrItem[MAX_ITEM_NUM];

//��ǰ��Ŀ����
static u32 s_iItemNum = 0;

//��ʾģʽ��0-������ʾ��1-��ʾͼƬ��
static u32 s_iShowMode = 0;

//�ļ��������֣���·����
static char s_arrFileCompleteName[MAX_NAME_LEN];

//��ǰ��ʾ���ļ����
static u32 s_iDirFilePos = 0;

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void  ShowSingleFile(StructFileInfo* file);              //��ʾ�����ļ���Ϣ
static void  FileIconMatch(StructFileInfo* file);               //ͼ��ƥ��
static void  CopyFileInfo(StructFileInfo* file, FILINFO* info); //�����ļ���Ϣ
static u32   GetFileNum(void);                 //ͳ�Ƶ�ǰĿ¼���ļ���������·����
static void  UpdateShow(void);                 //������ʾ
static u32   IsRoot(void);                     //У�鵱ǰĿ¼�Ƿ�Ϊ��Ŀ¼
static void  GotoParentDir(void);              //�л�����Ŀ¼
static void  GotoSubDir(char* dir);            //�л�����Ŀ¼
static char* CombiPathAndName(char* fileName); //����ǰ·�����ļ������
static void  ScanAllItem(void);                //ɨ�����е���ʾ��Ŀ
static void ScanProbar(void);                  //������ɨ��

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ShowSingleFile
* �������ܣ���ʾ�����ļ���Ϣ
* ���������file���ļ���Ϣ�ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ShowSingleFile(StructFileInfo* file)
{
  u32 x0, y0, x1, y1;

  //ѡ��ʱ������ɫ�л�
  if(1 == file->hasfocus)
  {
    x0 = file->x0;
    y0 = file->y0;
    x1 = file->x0 + file->width - 1;
    y1 = file->y0 + file->height - 1;
    GUIFillColor(x0, y0, x1, y1, RGB888ToRGB565B(FILL_COLOR));
  }

  //����ͼ��
  if(NULL != file->image.addr)
  {
    DisplayBMP(&file->image, file->x0, file->y0);
  }
  else
  {
    x0 = file->x0;
    y0 = file->y0;
    x1 = file->x0 + ICON_WIDTH - 1;
    y1 = file->y0 + ICON_HEIGHT - 1;
    GUIFillColor(x0, y0, x1, y1, GUI_COLOR_BLUE);
  }

  //��ʾ�ļ���
  x0 = file->x0 + ICON_WIDTH + ICON_TEXT_SPACE;
  y0 = file->y0;
  GUIDrawTextLine(x0, y0, (u32)file->fileName, NAME_FONT, NULL, GUI_COLOR_BLACK, 1, 
  ((FILE_INFO_WIDTH - ICON_WIDTH - ICON_TEXT_SPACE) / GetFontWidth(NAME_FONT)) - 1);

  //��ʾ����
  y0 = y0 + GetFontHeight(NAME_FONT) + TEXT_TEXT_SPACE;
  GUIDrawTextLine(x0, y0, (u32)file->fileData, DATE_FONT, NULL, GUI_COLOR_BLACK, 1, 
  ((FILE_INFO_WIDTH - ICON_WIDTH - ICON_TEXT_SPACE) / GetFontWidth(DATE_FONT)) - 1);

  //��ʾ�׶�����
  x0 = file->x0;
  y0 = file->y0 + ICON_HEIGHT + BUTTON_LINE_SPACE;
  x1 = file->x0 + file->width - 1;
  y1 = y0;
  GUIDrawLine(x0, y0, x1, y1, 1, RGB888ToRGB565B(LINE_COLOR), GUI_LINE_CIRCLE);
}

/*********************************************************************************************************
* �������ƣ�FileIconMatch
* �������ܣ�ͼ��ƥ��
* ���������file���ļ���Ϣ�ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void FileIconMatch(StructFileInfo* file)
{
  //Ĭ�ϴ洢���ļ�ϵͳ�У�����ʹ��͸�����㷨
  file->image.storageType = BMP_IN_FATFS;
  file->image.alphaType = BMP_UES_ALPHA;

  if(2 == file->fileType)
  {
    file->image.addr = FILESYS_DISK_ICON_DIR;
  }
  else if(1 == file->fileType)
  {
    file->image.addr = FILESYS_FOLDER_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "zip"))
  {
    file->image.addr = FILESYS_COMP_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "rar"))
  {
    file->image.addr = FILESYS_COMP_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "7z"))
  {
    file->image.addr = FILESYS_COMP_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "bmp"))
  {
    file->image.addr = FILESYS_IMAGE_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "jpg"))
  {
    file->image.addr = FILESYS_IMAGE_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "jpeg"))
  {
    file->image.addr = FILESYS_IMAGE_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "gif"))
  {
    file->image.addr = FILESYS_IMAGE_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "png"))
  {
    file->image.addr = FILESYS_IMAGE_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "mp3"))
  {
    file->image.addr = FILESYS_MUSIC_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "wav"))
  {
    file->image.addr = FILESYS_MUSIC_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "pdf"))
  {
    file->image.addr = FILESYS_PDF_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "txt"))
  {
    file->image.addr = FILESYS_TEXT_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "c"))
  {
    file->image.addr = FILESYS_TEXT_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "h"))
  {
    file->image.addr = FILESYS_TEXT_ICON_DIR;
  }
  else if(1 == FileSuffixCheck(file->fileName, "mp4"))
  {
    file->image.addr = FILESYS_VIDEO_ICON_DIR;
  }
  else
  {
    file->image.addr = FILESYS_FILE_ICON_DIR;
  }
}

/*********************************************************************************************************
* �������ƣ�CopyFileInfo
* �������ܣ������ļ���Ϣ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void CopyFileInfo(StructFileInfo* file, FILINFO* info)
{
  u32 i, year, month, data, hour, minute, second;
  char* name;

  //�����ļ���
  if(0 != info->lfname[0])
  {
    name = info->lfname;
  }
  else
  {
    name = info->fname;
  }
  i = 0;
  while(0 != name[i])
  {
    file->fileName[i] = name[i];
    i++;
  }
  file->fileName[i] = 0;

  //�����ļ�����
  year   = ((info->fdate & 0xFE00) >> 9 ) + 1980;
  month  = ((info->fdate & 0x01E0) >> 5 );
  data   = ((info->fdate & 0x001F) >> 0 );
  hour   = ((info->ftime & 0xF800) >> 11);
  minute = ((info->ftime & 0x07E0) >> 5 );
  second = ((info->ftime & 0x001F) >> 0 );

  //Ŀ¼
  if(info->fattrib & AM_DIR)
  {
    sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d", year, month, data, hour, minute, second);
  }

  //�ļ�
  else
  {
    //����1G
    if(info->fsize > 1024 * 1024 * 1024)
    {
      sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d  %.1fGB", 
      year, month, data, hour, minute, second, info->fsize / (1024.0 * 1024.0 * 1024.0));
    }

    //����1M
    else if(info->fsize > 1024 * 1024)
    {
      sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d  %.1fMB", 
      year, month, data, hour, minute, second, info->fsize / (1024.0 * 1024.0));
    }

    //����1K
    else if(info->fsize > 1024)
    {
      sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d  %.1fKB", 
      year, month, data, hour, minute, second, info->fsize / (1024.0));
    }

    //С��1K
    else
    {
      sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d  %dB",
      year, month, data, hour, minute, second, info->fsize);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�GetFileNum
* �������ܣ�ͳ�Ƶ�ǰĿ¼���ļ���������·����
* ���������void
* ���������void
* �� �� ֵ���ļ�����
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u32 GetFileNum(void)
{
  FRESULT result;         //�ļ��������ر���
  u32     fileNum;        //��ǰĿ¼���ļ���������·����

  //ͳ��
  fileNum = 0;
  f_opendir(&s_dirFileDir, s_arrCurrentDir);
  while(1)
  {
    result = f_readdir(&s_dirFileDir, &s_infoFileInfo);
    if((result != FR_OK) || (0 == s_infoFileInfo.fname[0]))
    {
      f_closedir(&s_dirFileDir);
      break;
    }
    else
    {
      fileNum++;
    }
  }

  return fileNum;
}

/*********************************************************************************************************
* �������ƣ�UpdateShow
* �������ܣ�������ʾ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void UpdateShow(void)
{
  FRESULT result;         //�ļ��������ر���
  u32     itemCnt;        //��ʾ����
  u32     x0, y0, x1, y1; //�������
  u32     fileNum;        //��ǰĿ¼���ļ���������·����
  u32     probarLen;      //����������
  u32     fileCnt;        //�ļ�����

  //��ʾ����ͼƬ������ͼƬռ��������Ļ��
  DisPlayBMPByIPA(0, 0, FILESYS_BACKGROUND_DIR);

  //ͳ�Ƶ�ǰĿ¼���ļ�����
  fileNum = GetFileNum();

  //ȷ�����������鳤��
  if(1 != IsRoot()){fileNum = fileNum + 1;}
  if(fileNum <= MAX_ITEM_NUM)
  {
    probarLen = PROBAR_HEIGHT;
  }
  else
  {
    probarLen = PROBAR_HEIGHT * MAX_ITEM_NUM / fileNum;
  }

  //ȷ����������������λ��
  if(1 == IsRoot()){fileNum = fileNum - 1;}
  x0 = PROBAR_X0;
  x1 = PROBAR_X1;
  y0 = PROBAR_Y0 + PROBAR_HEIGHT * s_iDirFilePos / fileNum;
  y1 = y0 + probarLen - 1;
  if(y1 > PROBAR_Y1)
  {
    y1 = PROBAR_Y1;
  }

  //���ƽ���������
  GUIFillColor(x0, y0, x1, y1, PROBAR_COLOR);

  //���ӳ��ļ���������
  s_infoFileInfo.lfname = s_arrFileLname;
  s_infoFileInfo.lfsize = MAX_NAME_LEN;

  //��ʾ��ǰĿ¼
  x0 = FILE_INFO_X0;
  y0 = DIR_SHOW_Y0 + (DIR_SHOW_HEIGHT - GetFontHeight(DIR_FONT)) / 2;
  GUIDrawTextLine(x0, y0, (u32)s_arrCurrentDir, DIR_FONT, NULL, GUI_COLOR_BLACK, 1, FILE_INFO_WIDTH / GetFontWidth(DIR_FONT));

  //�򿪵�ǰĿ¼
  f_opendir(&s_dirFileDir, s_arrCurrentDir);

  //��ʾ��һ��Ŀ¼����Ŀ¼�²���ʾ��
  if(1 != IsRoot())
  {
    s_arrItem[0].x0 = FILE_INFO_X0;
    s_arrItem[0].y0 = ITEM_SHOW_Y0;
    s_arrItem[0].width = FILE_INFO_WIDTH;
    s_arrItem[0].height = ICON_HEIGHT + BUTTON_LINE_SPACE;
    s_arrItem[0].fileType = 1;
    s_arrItem[0].dirType = 1;
    s_arrItem[0].hasfocus = 0;
    sprintf(s_arrItem[0].fileName, "..");
    sprintf(s_arrItem[0].fileData, "");
    FileIconMatch(&s_arrItem[0]);
    ShowSingleFile(&s_arrItem[0]);
    itemCnt = 1;
  }
  else
  {
    itemCnt = 0;
  }

  //��ѯĿ¼�������ļ�������ʾ
  fileCnt = 0;
  while(1)
  {
    //��ȡ�����ļ���Ϣ
    result = f_readdir(&s_dirFileDir, &s_infoFileInfo);
    if((result != FR_OK) || (0 == s_infoFileInfo.fname[0]))
    {
      f_closedir(&s_dirFileDir);
      break;
    }

    //δ����ʾλ�ã�ֱ��������һ��ѭ��
    if(fileCnt < s_iDirFilePos)
    {
      fileCnt++;
      continue;
    }

    //��Ļ����
    if(itemCnt >= MAX_ITEM_NUM)
    {
      f_closedir(&s_dirFileDir);
      break;
    }

    //���û�������
    s_arrItem[itemCnt].x0 = FILE_INFO_X0;
    s_arrItem[itemCnt].width = FILE_INFO_WIDTH;
    s_arrItem[itemCnt].height = ICON_HEIGHT + BUTTON_LINE_SPACE;
    s_arrItem[itemCnt].hasfocus = 0;
    if(0 == itemCnt)
    {
      s_arrItem[itemCnt].y0 = ITEM_SHOW_Y0;
    }
    else
    {
      s_arrItem[itemCnt].y0 = s_arrItem[itemCnt - 1].y0 + s_arrItem[itemCnt].height + ITEM_SHOW_SPACE;
    }

    //�����ļ���Ϣ
    CopyFileInfo(&s_arrItem[itemCnt], &s_infoFileInfo);

    //ƥ��Ŀ¼ͼ��
    if(s_infoFileInfo.fattrib & AM_DIR)
    {
      s_arrItem[itemCnt].fileType = 1;
      s_arrItem[itemCnt].dirType = 0;
    }

    //ƥ���ļ�ͼ��
    else
    {
      s_arrItem[itemCnt].fileType = 0;
    }
    FileIconMatch(&s_arrItem[itemCnt]);

    //�����ļ���ʾ
    ShowSingleFile(&s_arrItem[itemCnt]);

    //������Ŀ����
    itemCnt++;
  }

  //��¼��Ŀ����
  s_iItemNum = itemCnt;
}

/*********************************************************************************************************
* �������ƣ�IsRoot
* �������ܣ�У�鵱ǰĿ¼�Ƿ�Ϊ��Ŀ¼
* ���������void
* ���������void
* �� �� ֵ��0-��1-��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u32 IsRoot(void)
{
  u32 i, cnt;

  //����'/'������
  i = 0;
  cnt = 0;
  while(0 != s_arrCurrentDir[i])
  {
    if('/' == s_arrCurrentDir[i])
    {
      cnt++;
    }
    i++;
  }

  //��ǰĿ¼Ϊ��Ŀ¼
  if(0 == cnt)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*********************************************************************************************************
* �������ƣ�GotoParentDir
* �������ܣ��л�����Ŀ¼
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void GotoParentDir(void)
{
  u32 i, pos, cnt;

  //�������һ��'/'����λ��
  i = 0;
  pos = 0xFFFFFFFF;
  cnt = 0;
  while(0 != s_arrCurrentDir[i])
  {
    if('/' == s_arrCurrentDir[i])
    {
      pos = i;
      cnt++;
    }
    i++;
  }

  //��ǰĿ¼Ϊ��Ŀ¼
  if(0 == cnt)
  {
    return;
  }

  //�쳣
  if(0xFFFFFFFF == pos)
  {
    printf("�л�����һ��Ŀ¼ʧ��\r\n");
    while(1);
  }
  
  //ɾ����Ŀ¼��Ϣ
  s_arrCurrentDir[pos] = 0;

  //�ӵ�һ���ļ���ʼ��ʾ
  s_iDirFilePos = 0;

  //������ʾ
  UpdateShow();
}

/*********************************************************************************************************
* �������ƣ�GotoSubDir
* �������ܣ��л�����Ŀ¼
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void GotoSubDir(char* dir)
{
  u32 i;
  char* currentDir;

  //�����ַ�����β����λ��
  i = 0;
  while(0 != s_arrCurrentDir[i])
  {
    i++;
  }
  currentDir = s_arrCurrentDir + i;

  //�������'/'
  currentDir[0] = '/';
  currentDir++;

  //������Ŀ¼��Ϣ
  i = 0;
  while(0 != dir[i])
  {
    currentDir[i] = dir[i];
    i++;
  }
  currentDir[i] = 0;

  //�ӵ�һ���ļ���ʼ��ʾ
  s_iDirFilePos = 0;

  //������ʾ
  UpdateShow();
}

/*********************************************************************************************************
* �������ƣ�CombiPathAndName
* �������ܣ�����ǰ·�����ļ������
* ���������void
* ���������void
* �� �� ֵ��ת�����
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static char* CombiPathAndName(char* fileName)
{
  u32 i;
  char* path;

  //����·��
  i = 0;
  while(0 != s_arrCurrentDir[i])
  {
    s_arrFileCompleteName[i] = s_arrCurrentDir[i];
    i++;
  }
  path = s_arrFileCompleteName + i;

  //�������'/'
  path[0] = '/';
  path++;

  //�����ļ���
  i = 0;
  while(0 != fileName[i])
  {
    path[i] = fileName[i];
    i++;
  }
  path[i] = 0;

  return s_arrFileCompleteName;
}

/*********************************************************************************************************
* �������ƣ�ScanAllItem
* �������ܣ�ɨ�����е���ʾ��Ŀ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ScanAllItem(void)
{
  u32 i, x, y, x0, y0, x1, y1, width, height;
  StructBmpImage bmpImage;
  StructJpegImage jpegImage;

  //��ȡ��������
  if(1 == GUIGetTouch(&x, &y, 0))
  {
    //������ʾ�ļ�
    if(0 == s_iShowMode)
    {
      for(i = 0; i < s_iItemNum; i++)
      {
        x0 = s_arrItem[i].x0;
        y0 = s_arrItem[i].y0;
        x1 = s_arrItem[i].x0 + s_arrItem[i].width * 3 / 4 - 1;
        y1 = s_arrItem[i].y0 + s_arrItem[i].height - 1;
        if((x >= x0) && (x <= x1) && (y >= y0) && (y <= y1))
        {
          //���ظ�Ŀ¼
          if((1 == s_arrItem[i].fileType) && (1 == s_arrItem[i].dirType))
          {
            GotoParentDir();
          }

          //�л�����Ŀ¼
          else if(1 == s_arrItem[i].fileType)
          {
            GotoSubDir(s_arrItem[i].fileName);
          }

          //λͼ��ʾ
          else if(1 == FileSuffixCheck(s_arrItem[i].fileName, "bmp"))
          {
            bmpImage.addr = CombiPathAndName(s_arrItem[i].fileName);
            bmpImage.alphaType = BMP_UES_ALPHA;
            bmpImage.storageType = BMP_IN_FATFS;

            //��ȡͼ���С
            GetBMPSize(&bmpImage, &width, &height);

            //����ͼƬ��ʾλ��
            if((width <= LCD_PIXEL_HEIGHT) && (height <= LCD_PIXEL_WIDTH))
            {
              x0 = (LCD_PIXEL_HEIGHT - width) / 2;
              y0 = (LCD_PIXEL_WIDTH - height) / 2;
            }
            else
            {
              return;
            }

            //��ʾͼƬ
            LCDClear(LCD_COLOR_WHITE);
            DisplayBMP(&bmpImage, x0, y0);

            //���������ʾͼƬ
            s_iShowMode = 1;
          }

          //JPEGͼƬ��ʾ
          else if((1 == FileSuffixCheck(s_arrItem[i].fileName, "jpg")) || (1 == FileSuffixCheck(s_arrItem[i].fileName, "jpeg")))
          {

            //��ȡ�����ļ�������·����
            jpegImage.image = (u8*)CombiPathAndName(s_arrItem[i].fileName);

            //��ʾJPEGͼƬ
            LCDClear(LCD_COLOR_WHITE);
            DisplayJPEGInCentral(&jpegImage);

            //���������ʾͼƬ
            s_iShowMode = 1;
          }

          //�ȴ��û���ָ�뿪��Ļ
          WaitForTouchRelease();
        }
      }
    }

    //��ʾͼƬ��
    else
    {
      //�ȴ��û���ָ�뿪��Ļ
      WaitForTouchRelease();

      //�л�������ʾ
      s_iShowMode = 0;
      UpdateShow();
    }
  }
}

/*********************************************************************************************************
* �������ƣ�ScanProbar
* �������ܣ�������ɨ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ScanProbar(void)
{
  u32 x, y, x0, y0, x1, y1;
  u32 fileNum;

  //��ȡ��������
  fileNum = 0xFFFFFFFF;
  while(1 == GUIGetTouch(&x, &y, 0))
  {
    x0 = PROBAR_X0 - PROBAR_WIDTH * 2;
    y0 = PROBAR_Y0;
    x1 = PROBAR_X1;
    y1 = PROBAR_Y1;
    if((x >= x0) && (x <= x1) && (y >= y0) && (y <= y1))
    {
      //��ȡ��ǰ·�����ļ�����
      if(0xFFFFFFFF == fileNum)
      {
        fileNum = GetFileNum();
      }

      //�����ļ���ʾλ��
      s_iDirFilePos = fileNum * (y - y0) / PROBAR_HEIGHT;

      //������ʾ
      UpdateShow();

      break;
    }
    else
    {
      break;
    }
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUIFileSys
* �������ܣ������ļ�ϵͳ��ʾ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateGUIFileSys(void)
{
  //������ʾ
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //������ʾ
  UpdateShow();
}

/*********************************************************************************************************
* �������ƣ�DeleteGUIFileSys
* �������ܣ�ɾ���ļ�ϵͳ��ʾ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUIFileSys(void)
{

}

/*********************************************************************************************************
* �������ƣ�GUIFileSysPoll
* �������ܣ��ļ�ϵͳ��ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIFileSysPoll(void)
{
  ScanAllItem();
  ScanProbar();
}
