/*********************************************************************************************************
* 模块名称：GUIFileSys.c
* 摘    要：文件系统显示模块
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
*                                              宏定义
*********************************************************************************************************/
#define FILE_INFO_X0      (32)
#define FILE_INFO_X1      (448)
#define FILE_INFO_WIDTH   (FILE_INFO_X1 - FILE_INFO_X0 + 1)

#define DIR_SHOW_Y0       (52)
#define DIR_SHOW_HEIGHT   (34)
#define DIR_FILL_COLOR    ((242 << 16) | (242 << 8) | (242))
#define DIR_FONT          (GUI_FONT_ASCII_24)

#define ICON_WIDTH        (40)     //图标宽度
#define ICON_HEIGHT       (40)     //图标高度
#define ICON_TEXT_SPACE   (5)      //图片与字符串显示的间隙
#define TEXT_TEXT_SPACE   (1)      //上下两个字符串之间的间隙
#define BUTTON_LINE_SPACE (2)      //底部线条与图片的间隙
#define MAX_NAME_LEN      _MAX_LFN //最长文件名
#define NAME_FONT         (GUI_FONT_ASCII_24)
#define DATE_FONT         (GUI_FONT_ASCII_12)
#define LINE_COLOR        ((217 << 16) | (217 << 8) | (217)) //底端线条颜色（RGB888）
#define FILL_COLOR        ((242 << 16) | (242 << 8) | (242)) //选中填充颜色（RGB888）

#define ITEM_SHOW_SPACE   (10)
#define ITEM_SHOW_Y0      (DIR_SHOW_Y0 + DIR_SHOW_HEIGHT + ITEM_SHOW_SPACE)
#define ITEM_HEIGHT       (ICON_HEIGHT + BUTTON_LINE_SPACE)
#define MAX_ITEM_NUM      ((LCD_PIXEL_WIDTH - ITEM_SHOW_Y0) / (ITEM_HEIGHT + ITEM_SHOW_SPACE))

//进度条相关定义
#define PROBAR_X0         (452)
#define PROBAR_Y0         (140)
#define PROBAR_WIDTH      (15)
#define PROBAR_HEIGHT     (620)
#define PROBAR_X1         (PROBAR_X0 + PROBAR_WIDTH - 1)
#define PROBAR_Y1         (PROBAR_Y0 + PROBAR_HEIGHT - 1)
//#define PROBAR_COLOR      RGB888ToRGB565B((181 << 16) | (210 << 8) | (236 << 0))
#define PROBAR_COLOR      RGB888ToRGB565B((46 << 16) | (117 << 8) | (182 << 0))

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
//文件信息结构体
typedef struct 
{
  u32  x0;       //起始坐标
  u32  y0;       //起始坐标
  u32  width;    //宽度
  u32  height;   //高度
  u32  fileType; //文件类型，0-普通文件，1-路径，2-磁盘文件
  u32  dirType;  //0-子目录，1-父目录
  u32  hasfocus; //有无焦点，0-无焦点，1-有焦点
  char fileName[MAX_NAME_LEN]; //文件名，不含路径
  char fileData[MAX_NAME_LEN]; //文件信息
  StructBmpImage image;        //文件图标
}StructFileInfo;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static char    s_arrCurrentDir[MAX_NAME_LEN] = "0:";
static char    s_arrFileLname[MAX_NAME_LEN];
static DIR     s_dirFileDir;   //当前路径
static FILINFO s_infoFileInfo; //当前文件信息

//项目控件（每一行一个项目）
static StructFileInfo s_arrItem[MAX_ITEM_NUM];

//当前项目数量
static u32 s_iItemNum = 0;

//显示模式，0-正常显示，1-显示图片中
static u32 s_iShowMode = 0;

//文件完整名字（含路径）
static char s_arrFileCompleteName[MAX_NAME_LEN];

//当前显示的文件起点
static u32 s_iDirFilePos = 0;

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void  ShowSingleFile(StructFileInfo* file);              //显示单个文件信息
static void  FileIconMatch(StructFileInfo* file);               //图标匹配
static void  CopyFileInfo(StructFileInfo* file, FILINFO* info); //拷贝文件信息
static u32   GetFileNum(void);                 //统计当前目录下文件数量（含路径）
static void  UpdateShow(void);                 //更新显示
static u32   IsRoot(void);                     //校验当前目录是否为根目录
static void  GotoParentDir(void);              //切换到父目录
static void  GotoSubDir(char* dir);            //切换到子目录
static char* CombiPathAndName(char* fileName); //将当前路径和文件名组合
static void  ScanAllItem(void);                //扫描所有的显示项目
static void ScanProbar(void);                  //进度条扫描

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ShowSingleFile
* 函数功能：显示单个文件信息
* 输入参数：file：文件信息结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ShowSingleFile(StructFileInfo* file)
{
  u32 x0, y0, x1, y1;

  //选中时背景颜色切换
  if(1 == file->hasfocus)
  {
    x0 = file->x0;
    y0 = file->y0;
    x1 = file->x0 + file->width - 1;
    y1 = file->y0 + file->height - 1;
    GUIFillColor(x0, y0, x1, y1, RGB888ToRGB565B(FILL_COLOR));
  }

  //绘制图标
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

  //显示文件名
  x0 = file->x0 + ICON_WIDTH + ICON_TEXT_SPACE;
  y0 = file->y0;
  GUIDrawTextLine(x0, y0, (u32)file->fileName, NAME_FONT, NULL, GUI_COLOR_BLACK, 1, 
  ((FILE_INFO_WIDTH - ICON_WIDTH - ICON_TEXT_SPACE) / GetFontWidth(NAME_FONT)) - 1);

  //显示日期
  y0 = y0 + GetFontHeight(NAME_FONT) + TEXT_TEXT_SPACE;
  GUIDrawTextLine(x0, y0, (u32)file->fileData, DATE_FONT, NULL, GUI_COLOR_BLACK, 1, 
  ((FILE_INFO_WIDTH - ICON_WIDTH - ICON_TEXT_SPACE) / GetFontWidth(DATE_FONT)) - 1);

  //显示底端线条
  x0 = file->x0;
  y0 = file->y0 + ICON_HEIGHT + BUTTON_LINE_SPACE;
  x1 = file->x0 + file->width - 1;
  y1 = y0;
  GUIDrawLine(x0, y0, x1, y1, 1, RGB888ToRGB565B(LINE_COLOR), GUI_LINE_CIRCLE);
}

/*********************************************************************************************************
* 函数名称：FileIconMatch
* 函数功能：图标匹配
* 输入参数：file：文件信息结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void FileIconMatch(StructFileInfo* file)
{
  //默认存储在文件系统中，并且使用透明度算法
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
* 函数名称：CopyFileInfo
* 函数功能：拷贝文件信息
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void CopyFileInfo(StructFileInfo* file, FILINFO* info)
{
  u32 i, year, month, data, hour, minute, second;
  char* name;

  //拷贝文件名
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

  //解析文件日期
  year   = ((info->fdate & 0xFE00) >> 9 ) + 1980;
  month  = ((info->fdate & 0x01E0) >> 5 );
  data   = ((info->fdate & 0x001F) >> 0 );
  hour   = ((info->ftime & 0xF800) >> 11);
  minute = ((info->ftime & 0x07E0) >> 5 );
  second = ((info->ftime & 0x001F) >> 0 );

  //目录
  if(info->fattrib & AM_DIR)
  {
    sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d", year, month, data, hour, minute, second);
  }

  //文件
  else
  {
    //大于1G
    if(info->fsize > 1024 * 1024 * 1024)
    {
      sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d  %.1fGB", 
      year, month, data, hour, minute, second, info->fsize / (1024.0 * 1024.0 * 1024.0));
    }

    //大于1M
    else if(info->fsize > 1024 * 1024)
    {
      sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d  %.1fMB", 
      year, month, data, hour, minute, second, info->fsize / (1024.0 * 1024.0));
    }

    //大于1K
    else if(info->fsize > 1024)
    {
      sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d  %.1fKB", 
      year, month, data, hour, minute, second, info->fsize / (1024.0));
    }

    //小于1K
    else
    {
      sprintf(file->fileData, "%02d/%02d/%02d %02d:%02d:%02d  %dB",
      year, month, data, hour, minute, second, info->fsize);
    }
  }
}

/*********************************************************************************************************
* 函数名称：GetFileNum
* 函数功能：统计当前目录下文件数量（含路径）
* 输入参数：void
* 输出参数：void
* 返 回 值：文件数量
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u32 GetFileNum(void)
{
  FRESULT result;         //文件操作返回变量
  u32     fileNum;        //当前目录下文件数量（含路径）

  //统计
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
* 函数名称：UpdateShow
* 函数功能：更新显示
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void UpdateShow(void)
{
  FRESULT result;         //文件操作返回变量
  u32     itemCnt;        //显示计数
  u32     x0, y0, x1, y1; //坐标变量
  u32     fileNum;        //当前目录下文件数量（含路径）
  u32     probarLen;      //进度条长度
  u32     fileCnt;        //文件计数

  //显示背景图片（背景图片占满整个屏幕）
  DisPlayBMPByIPA(0, 0, FILESYS_BACKGROUND_DIR);

  //统计当前目录下文件数量
  fileNum = GetFileNum();

  //确定进度条方块长度
  if(1 != IsRoot()){fileNum = fileNum + 1;}
  if(fileNum <= MAX_ITEM_NUM)
  {
    probarLen = PROBAR_HEIGHT;
  }
  else
  {
    probarLen = PROBAR_HEIGHT * MAX_ITEM_NUM / fileNum;
  }

  //确定进度条方块坐标位置
  if(1 == IsRoot()){fileNum = fileNum - 1;}
  x0 = PROBAR_X0;
  x1 = PROBAR_X1;
  y0 = PROBAR_Y0 + PROBAR_HEIGHT * s_iDirFilePos / fileNum;
  y1 = y0 + probarLen - 1;
  if(y1 > PROBAR_Y1)
  {
    y1 = PROBAR_Y1;
  }

  //绘制进度条方块
  GUIFillColor(x0, y0, x1, y1, PROBAR_COLOR);

  //连接长文件名缓冲区
  s_infoFileInfo.lfname = s_arrFileLname;
  s_infoFileInfo.lfsize = MAX_NAME_LEN;

  //显示当前目录
  x0 = FILE_INFO_X0;
  y0 = DIR_SHOW_Y0 + (DIR_SHOW_HEIGHT - GetFontHeight(DIR_FONT)) / 2;
  GUIDrawTextLine(x0, y0, (u32)s_arrCurrentDir, DIR_FONT, NULL, GUI_COLOR_BLACK, 1, FILE_INFO_WIDTH / GetFontWidth(DIR_FONT));

  //打开当前目录
  f_opendir(&s_dirFileDir, s_arrCurrentDir);

  //显示上一级目录（根目录下不显示）
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

  //查询目录下所有文件，并显示
  fileCnt = 0;
  while(1)
  {
    //读取单个文件信息
    result = f_readdir(&s_dirFileDir, &s_infoFileInfo);
    if((result != FR_OK) || (0 == s_infoFileInfo.fname[0]))
    {
      f_closedir(&s_dirFileDir);
      break;
    }

    //未到显示位置，直接跳到下一个循环
    if(fileCnt < s_iDirFilePos)
    {
      fileCnt++;
      continue;
    }

    //屏幕已满
    if(itemCnt >= MAX_ITEM_NUM)
    {
      f_closedir(&s_dirFileDir);
      break;
    }

    //设置基本参数
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

    //拷贝文件信息
    CopyFileInfo(&s_arrItem[itemCnt], &s_infoFileInfo);

    //匹配目录图标
    if(s_infoFileInfo.fattrib & AM_DIR)
    {
      s_arrItem[itemCnt].fileType = 1;
      s_arrItem[itemCnt].dirType = 0;
    }

    //匹配文件图标
    else
    {
      s_arrItem[itemCnt].fileType = 0;
    }
    FileIconMatch(&s_arrItem[itemCnt]);

    //单个文件显示
    ShowSingleFile(&s_arrItem[itemCnt]);

    //更新项目计数
    itemCnt++;
  }

  //记录项目数量
  s_iItemNum = itemCnt;
}

/*********************************************************************************************************
* 函数名称：IsRoot
* 函数功能：校验当前目录是否为根目录
* 输入参数：void
* 输出参数：void
* 返 回 值：0-否，1-是
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u32 IsRoot(void)
{
  u32 i, cnt;

  //查验'/'的数量
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

  //当前目录为根目录
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
* 函数名称：GotoParentDir
* 函数功能：切换到父目录
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void GotoParentDir(void)
{
  u32 i, pos, cnt;

  //查找最后一个'/'所在位置
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

  //当前目录为根目录
  if(0 == cnt)
  {
    return;
  }

  //异常
  if(0xFFFFFFFF == pos)
  {
    printf("切换到上一级目录失败\r\n");
    while(1);
  }
  
  //删除子目录信息
  s_arrCurrentDir[pos] = 0;

  //从第一个文件开始显示
  s_iDirFilePos = 0;

  //更新显示
  UpdateShow();
}

/*********************************************************************************************************
* 函数名称：GotoSubDir
* 函数功能：切换到子目录
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void GotoSubDir(char* dir)
{
  u32 i;
  char* currentDir;

  //查找字符串结尾所在位置
  i = 0;
  while(0 != s_arrCurrentDir[i])
  {
    i++;
  }
  currentDir = s_arrCurrentDir + i;

  //补充符号'/'
  currentDir[0] = '/';
  currentDir++;

  //拷贝子目录信息
  i = 0;
  while(0 != dir[i])
  {
    currentDir[i] = dir[i];
    i++;
  }
  currentDir[i] = 0;

  //从第一个文件开始显示
  s_iDirFilePos = 0;

  //更新显示
  UpdateShow();
}

/*********************************************************************************************************
* 函数名称：CombiPathAndName
* 函数功能：将当前路径和文件名组合
* 输入参数：void
* 输出参数：void
* 返 回 值：转换结果
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static char* CombiPathAndName(char* fileName)
{
  u32 i;
  char* path;

  //拷贝路径
  i = 0;
  while(0 != s_arrCurrentDir[i])
  {
    s_arrFileCompleteName[i] = s_arrCurrentDir[i];
    i++;
  }
  path = s_arrFileCompleteName + i;

  //补充符号'/'
  path[0] = '/';
  path++;

  //拷贝文件名
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
* 函数名称：ScanAllItem
* 函数功能：扫描所有的显示项目
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ScanAllItem(void)
{
  u32 i, x, y, x0, y0, x1, y1, width, height;
  StructBmpImage bmpImage;
  StructJpegImage jpegImage;

  //获取触屏坐标
  if(1 == GUIGetTouch(&x, &y, 0))
  {
    //正常显示文件
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
          //返回父目录
          if((1 == s_arrItem[i].fileType) && (1 == s_arrItem[i].dirType))
          {
            GotoParentDir();
          }

          //切换到子目录
          else if(1 == s_arrItem[i].fileType)
          {
            GotoSubDir(s_arrItem[i].fileName);
          }

          //位图显示
          else if(1 == FileSuffixCheck(s_arrItem[i].fileName, "bmp"))
          {
            bmpImage.addr = CombiPathAndName(s_arrItem[i].fileName);
            bmpImage.alphaType = BMP_UES_ALPHA;
            bmpImage.storageType = BMP_IN_FATFS;

            //获取图像大小
            GetBMPSize(&bmpImage, &width, &height);

            //计算图片显示位置
            if((width <= LCD_PIXEL_HEIGHT) && (height <= LCD_PIXEL_WIDTH))
            {
              x0 = (LCD_PIXEL_HEIGHT - width) / 2;
              y0 = (LCD_PIXEL_WIDTH - height) / 2;
            }
            else
            {
              return;
            }

            //显示图片
            LCDClear(LCD_COLOR_WHITE);
            DisplayBMP(&bmpImage, x0, y0);

            //标记正在显示图片
            s_iShowMode = 1;
          }

          //JPEG图片显示
          else if((1 == FileSuffixCheck(s_arrItem[i].fileName, "jpg")) || (1 == FileSuffixCheck(s_arrItem[i].fileName, "jpeg")))
          {

            //获取完整文件名（含路径）
            jpegImage.image = (u8*)CombiPathAndName(s_arrItem[i].fileName);

            //显示JPEG图片
            LCDClear(LCD_COLOR_WHITE);
            DisplayJPEGInCentral(&jpegImage);

            //标记正在显示图片
            s_iShowMode = 1;
          }

          //等待用户手指离开屏幕
          WaitForTouchRelease();
        }
      }
    }

    //显示图片中
    else
    {
      //等待用户手指离开屏幕
      WaitForTouchRelease();

      //切回正常显示
      s_iShowMode = 0;
      UpdateShow();
    }
  }
}

/*********************************************************************************************************
* 函数名称：ScanProbar
* 函数功能：进度条扫描
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ScanProbar(void)
{
  u32 x, y, x0, y0, x1, y1;
  u32 fileNum;

  //获取触屏坐标
  fileNum = 0xFFFFFFFF;
  while(1 == GUIGetTouch(&x, &y, 0))
  {
    x0 = PROBAR_X0 - PROBAR_WIDTH * 2;
    y0 = PROBAR_Y0;
    x1 = PROBAR_X1;
    y1 = PROBAR_Y1;
    if((x >= x0) && (x <= x1) && (y >= y0) && (y <= y1))
    {
      //获取当前路径下文件数量
      if(0xFFFFFFFF == fileNum)
      {
        fileNum = GetFileNum();
      }

      //更新文件显示位置
      s_iDirFilePos = fileNum * (y - y0) / PROBAR_HEIGHT;

      //更新显示
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
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUIFileSys
* 函数功能：创建文件系统显示界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIFileSys(void)
{
  //竖屏显示
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //更新显示
  UpdateShow();
}

/*********************************************************************************************************
* 函数名称：DeleteGUIFileSys
* 函数功能：删除文件系统显示界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUIFileSys(void)
{

}

/*********************************************************************************************************
* 函数名称：GUIFileSysPoll
* 函数功能：文件系统轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIFileSysPoll(void)
{
  ScanAllItem();
  ScanProbar();
}
