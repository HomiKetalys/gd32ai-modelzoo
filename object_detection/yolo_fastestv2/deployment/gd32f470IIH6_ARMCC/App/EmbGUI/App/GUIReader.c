/*********************************************************************************************************
* 模块名称：GUIReader.c
* 摘    要：图书模块
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
#include "GUIReader.h"
#include "BookByteRead.h"
#include "GUIPlatform.h"
#include "ButtonWidget.h"
#include "TLILCD.h"
#include "BMP.h"
#include "Common.h"
#include "stdio.h"
#include "ff.h"
#include "Malloc.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define BOOK_X0     (6)    //显示区域x0
#define BOOK_Y0     (62)   //显示区域y0
#define BOOK_X1     (474)  //显示区域x1
#define BOOK_Y1     (734)  //显示区域y1
#define FONT_WIDTH  (12)   //显示字体宽度
#define FONT_HEIGHT (30)   //显示字体高度
#define MAX_LINE_NUM ((BOOK_Y1 - BOOK_Y0) / FONT_HEIGHT) //最大行数

//最大上一页数量
#define MAX_PREV_PAGE (64)

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
typedef enum
{
  GUI_BUTTON_PREVIOUS = 0, //前一页按键按键，必须是从0
  GUI_BUTTON_NEXT,         //后一页按键
  GUI_BUTTON_MAX,          //按键数量
  GUI_BUTTON_NONE,         //无效按键
}EnumGUIButtons;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX]; //触摸按键
static EnumGUIButtons     s_enumButtonPress;           //用来标记那个按键按下

static u32          s_iBackColor;    //背景颜色
static char         s_iEndFlag = 0;  //文本显示完全标志位
static char         s_iLastChar = 0; //上一个未显示出来的字符
static u32          s_arrPrevPosition[MAX_PREV_PAGE]; //上一页起始位置(0xFFFFFFFF表示无意义)

//书名
static char* s_pBookName = "西游记";

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void ButtonPressCallback(void* button);  //按键回调函数
static EnumGUIButtons ScanGUIButton(void);      //按键扫描
static void NewPage(void);      //显示新的一页
static void PreviousPage(void); //显示上一页
static void NextPage(void);     //下一页

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ButtonPressCallback
* 函数功能：按键回调函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ButtonPressCallback(void* button)
{
  EnumGUIButtons i;

  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    if((StructButtonWidget*)button == &s_arrButton[i])
    {
      s_enumButtonPress = i;
      return;
    }
  }
}

/*********************************************************************************************************
* 函数名称：ScanGUIButton
* 函数功能：按键扫描
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：每隔20毫秒执行一次
*********************************************************************************************************/
static EnumGUIButtons ScanGUIButton(void)
{
  EnumGUIButtons i;

  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    s_enumButtonPress = GUI_BUTTON_NONE;
    ScanButtonWidget(&s_arrButton[i]);

    if(GUI_BUTTON_NONE != s_enumButtonPress)
    {
      return s_enumButtonPress;
    }
  }

  return GUI_BUTTON_NONE;
}

/*********************************************************************************************************
* 函数名称：NewPage
* 函数功能：显示新的一页
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void NewPage(void)
{
  u8  newchar;                //当前字节
  u16 cnChar;                 //中文符号
  u32 lineCnt;                //当前行位置
  u32 rowCnt;                 //当前列位置
  u32 x;                      //字符显示横坐标
  u32 y;                      //字符显示纵坐标
  u32 visibleLen;             //当前字符开始往后有多少个可视字符
  u32 newParaFlag;            //新段标志位（用于非段落开头空行不显示）

  //清除显示
  GUIFillColor(BOOK_X0, BOOK_Y0, BOOK_X1, BOOK_Y1, s_iBackColor);

  //显示上一个未打印出来的字符
  if((s_iLastChar >= ' ') && (s_iLastChar <= '~'))
  {
    rowCnt = 0;
    lineCnt = 0;
    x = BOOK_X0 + FONT_WIDTH * rowCnt;
    y = BOOK_Y0 + FONT_HEIGHT * lineCnt;
    GUIDrawChar(x, y, s_iLastChar, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1);
    rowCnt = 1;
  }
  else
  {
    rowCnt = 0;
    lineCnt = 0;
  }

  //显示一整页内容
  newchar = 0;
  newParaFlag = 0;
  s_iLastChar = 0;
  while(1)
  {
    cnChar = 0;

    //从缓冲区中读取1字节数据
    if(0 == ReadBookByte((char*)&newchar, &visibleLen))
    {
      s_iEndFlag = 1;
      return;
    }

    //回车换行符号
    if('\r' == newchar)
    {
      rowCnt = 0;
    }

    //换行
    else if('\n' == newchar)
    {
      rowCnt = 0;
      lineCnt = lineCnt + 1;
      if(lineCnt >= MAX_LINE_NUM)
      {
        return;
      }
      newParaFlag = 1;
    }

    //英文
    if((newchar >= ' ') && (newchar <= '~'))
    {
      //检查当前行是否足够显示整个单词
      if((newchar != ' ') && ((BOOK_X0 + FONT_WIDTH * (rowCnt + visibleLen)) > (BOOK_X1 - FONT_WIDTH)))
      {
        rowCnt = 0;
        lineCnt = lineCnt + 1;
        if(lineCnt >= MAX_LINE_NUM)
        {
          s_iLastChar = newchar;
          return;
        }
      }

      //非新段首行空格不显示
      if((0 == rowCnt) && (0 == newParaFlag) && (' ' == newchar))
      {
        continue;
      }

      x = BOOK_X0 + FONT_WIDTH * rowCnt;
      y = BOOK_Y0 + FONT_HEIGHT * lineCnt;
      GUIDrawChar(x, y, newchar, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1);
    }

    //中文
    else if(newchar >= 0x81)
    {
      //保存高字节
      cnChar = newchar << 8;

      //从缓冲区中读取1字节数据
      if(0 == ReadBookByte((char*)&newchar, &visibleLen))
      {
        s_iEndFlag = 1;
        return;
      }

      //组合成一个完整的汉字内码
      cnChar = cnChar | newchar;

      //显示
      x = BOOK_X0 + FONT_WIDTH * rowCnt;
      y = BOOK_Y0 + FONT_HEIGHT * lineCnt;
      //ShowCNChar(x, y, cnChar, GUI_COLOR_BLACK, NULL, 24, 1);
      GUIDrawChar(x, y, cnChar, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1);
    }

    //更新列计数（中文占2列）
    if(0 == cnChar)
    {
      rowCnt = rowCnt + 1;
    }
    else
    {
      rowCnt = rowCnt + 2;
    }
    x = BOOK_X0 + FONT_WIDTH * rowCnt;
    if(x > (BOOK_X1 - 2 * FONT_WIDTH))
    {
      rowCnt = 0;
    }

    //更新行计数
    if(0 == rowCnt)
    {
      lineCnt = lineCnt + 1;
      if(lineCnt >= MAX_LINE_NUM)
      {
        return;
      }
    }

    //清除新段标志位
    newParaFlag = 0;
  }
}

/*********************************************************************************************************
* 函数名称：PreviousPage
* 函数功能：上一页
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void PreviousPage(void)
{
  u32  i; //循环变量

  //刷新上一页位置数据
  if(0xFFFFFFFF != s_arrPrevPosition[MAX_PREV_PAGE - 2])
  {
    for(i = (MAX_PREV_PAGE - 1); i > 0; i--)
    {
      s_arrPrevPosition[i] = s_arrPrevPosition[i - 1];
    }
    s_arrPrevPosition[0] = 0xFFFFFFFF;
  }

  //上一页有意义
  if(0xFFFFFFFF != s_arrPrevPosition[MAX_PREV_PAGE - 1])
  {
    //成功设置读写位置
    if(1 == SetPosision(s_arrPrevPosition[MAX_PREV_PAGE - 1]))
    {
      s_iEndFlag = 0;
      s_iLastChar = 0;

      //刷新新的一页
      NewPage();
    }
  }
}

/*********************************************************************************************************
* 函数名称：NextPage
* 函数功能：下一页
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void NextPage(void)
{
  u32  i; //循环变量

  //文本已全部显示，直接退出
  if(1 == s_iEndFlag)
  {
    return;
  }

  //保存上一页位置
  if(0xFFFFFFFF == s_arrPrevPosition[MAX_PREV_PAGE - 1])
  {
    s_arrPrevPosition[MAX_PREV_PAGE - 1] = GetBytePosition();
    s_arrPrevPosition[MAX_PREV_PAGE - 2] = GetBytePosition();
  }
  else
  {
    for(i = 0; i < (MAX_PREV_PAGE - 1); i++)
    {
      s_arrPrevPosition[i] = s_arrPrevPosition[i + 1];
    }
    s_arrPrevPosition[MAX_PREV_PAGE - 1] = GetBytePosition();
  }

  //刷新新的一页
  NewPage();
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUIReader
* 函数功能：创建图书
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIReader(void)
{
  static StructBmpImage s_structKeyPreviousPressImage;
  static StructBmpImage s_structKeyPreviousReleaseImage;
  static StructBmpImage s_structKeyNextPressImage;
  static StructBmpImage s_structKeyNextReleaseImage;

  u32 len, max, total, begin;

  //LCD竖屏显示
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //显示背景图片
  DisPlayBMPByIPA(0, 0, READER_BACKGROUND_DIR);

  //标记无按键按下
  s_enumButtonPress = GUI_BUTTON_NONE;

  //初始化位图
  s_structKeyPreviousPressImage.alphaType   = BMP_UES_ALPHA;
  s_structKeyPreviousPressImage.alphaGate   = 200;
  s_structKeyPreviousPressImage.storageType = BMP_IN_FATFS;
  s_structKeyPreviousPressImage.addr        = (void*)READER_PREVIOUS_PRESS_DIR;

  s_structKeyPreviousReleaseImage.alphaType   = BMP_UES_ALPHA;
  s_structKeyPreviousReleaseImage.alphaGate   = 200;
  s_structKeyPreviousReleaseImage.storageType = BMP_IN_FATFS;
  s_structKeyPreviousReleaseImage.addr        = (void*)READER_PREVIOUS_RELEASE_DIR;

  s_structKeyNextPressImage.alphaType   = BMP_UES_ALPHA;
  s_structKeyNextPressImage.alphaGate   = 200;
  s_structKeyNextPressImage.storageType = BMP_IN_FATFS;
  s_structKeyNextPressImage.addr        = (void*)READER_NEXT_PRESS_DIR;

  s_structKeyNextReleaseImage.alphaType   = BMP_UES_ALPHA;
  s_structKeyNextReleaseImage.alphaGate   = 200;
  s_structKeyNextReleaseImage.storageType = BMP_IN_FATFS;
  s_structKeyNextReleaseImage.addr        = (void*)READER_NEXT_RELEASE_DIR;

  //创建前一页按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PREVIOUS]);
  s_arrButton[GUI_BUTTON_PREVIOUS].x0 = 10;
  s_arrButton[GUI_BUTTON_PREVIOUS].y0 = 740;
  s_arrButton[GUI_BUTTON_PREVIOUS].width = 130;
  s_arrButton[GUI_BUTTON_PREVIOUS].height = 45;
  s_arrButton[GUI_BUTTON_PREVIOUS].textX0 = 35;
  s_arrButton[GUI_BUTTON_PREVIOUS].textY0 = 10;
  s_arrButton[GUI_BUTTON_PREVIOUS].text = "";
  s_arrButton[GUI_BUTTON_PREVIOUS].pressBackgroudImage = (void*)(&s_structKeyPreviousPressImage);
  s_arrButton[GUI_BUTTON_PREVIOUS].releaseBackgroudImage = (void*)(&s_structKeyPreviousReleaseImage);
  s_arrButton[GUI_BUTTON_PREVIOUS].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PREVIOUS]);

  //创建后一页按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_NEXT]);
  s_arrButton[GUI_BUTTON_NEXT].x0 = 340;
  s_arrButton[GUI_BUTTON_NEXT].y0 = 740;
  s_arrButton[GUI_BUTTON_NEXT].width = 130;
  s_arrButton[GUI_BUTTON_NEXT].height = 45;
  s_arrButton[GUI_BUTTON_NEXT].textX0 = 35;
  s_arrButton[GUI_BUTTON_NEXT].textY0 = 10;
  s_arrButton[GUI_BUTTON_NEXT].text = "";
  s_arrButton[GUI_BUTTON_NEXT].pressBackgroudImage = (void*)(&s_structKeyNextPressImage);
  s_arrButton[GUI_BUTTON_NEXT].releaseBackgroudImage = (void*)(&s_structKeyNextReleaseImage);
  s_arrButton[GUI_BUTTON_NEXT].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_NEXT]);

  //计算书名长度
  len = 0;
  while((0 != s_pBookName[len]) && ('\r' != s_pBookName[len]) && ('\n' != s_pBookName[len]))
  {
    len++;
  }

  //计算屏幕一行最多显示多少个字符
  max = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer] / 12;

  //所需的像素点总数
  total = 12 * len;

  //计算起始显示像素点
  begin = (g_structTLILCDDev.width[g_structTLILCDDev.currentLayer] - total) / 2;

  //显示书名
  GUIDrawTextLine(begin, 15, (u32)s_pBookName, GUI_FONT_ASCII_24, NULL, GUI_COLOR_WHITE, 1, max);

  //获取阅读背景颜色（使用纯色背景）
  s_iBackColor = GUIReadPoint((BOOK_X0 + BOOK_X1) / 2, (BOOK_Y0 + BOOK_Y1) / 2);

  //先刷新一页出来
  NextPage();
}

/*********************************************************************************************************
* 函数名称：DeleteGUIReader
* 函数功能：删除图书
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUIReader(void)
{

}

/*********************************************************************************************************
* 函数名称：GUIReaderPoll
* 函数功能：图书轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIReaderPoll(void)
{
  EnumGUIButtons button;

  //按键扫描
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //前一页
    if(GUI_BUTTON_PREVIOUS == button)
    {
      PreviousPage();
    }

    //后一页
    else if(GUI_BUTTON_NEXT == button)
    {
      NextPage();
    }
  }
}

/*********************************************************************************************************
* 函数名称：CreatReadProgressFile
* 函数功能：创建保存阅读进度文件
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、打开文件时文件变量FIL一定要是静态变量或动态内存分配的，否则会卡死，原因未知
*          1、FA_READ         ：指定对文件的读取权限。可以从文件中读取数据
*          2、FA_WRITE        ：指定对文件的写入访问权限。可以将数据写入该文件。与FA_READ组合以进行读写访问
*          3、FA_OPEN_EXISTING：打开一个文件。如果文件不存在，该函数将失败。（默认）
*          4、FA_CREATE_NEW   ：创建一个新文件。如果文件已存在，那么函数将失败并返回FR_EXIST
*          5、FA_CREATE_ALWAYS：创建一个新文件。如果文件已存在，那么它将被截断和覆盖
*          6、FA_OPEN_ALWAYS  ：打开文件（如果存在）。否则，将创建一个新文件
*          7、FA_OPEN_APPEND  ：与FA_OPEN_ALWAYS相同，只是读/写指针设置在文件末尾
*********************************************************************************************************/
void CreatReadProgressFile(void)
{
  static FIL s_fileProgressFile; //进度缓存文件
  DIR        progressDir;        //目标路径
  FRESULT    result;             //文件操作返回变量
  
  //校验进度缓存路径是否存在，若不存在则创建该路径
  result = f_opendir(&progressDir,"0:/book/progress");
  if(FR_NO_PATH == result)
  {
    f_mkdir("0:/book/progress");
  }
  else
  {
    f_closedir(&progressDir);
  }

  //检查文件是否存在，如不存在则创建
  result = f_open(&s_fileProgressFile, "0:/book/progress/progress.txt", FA_CREATE_NEW | FA_READ);
  if(FR_OK != result)
  {
    printf("CreatReadProgressFile：文件已存在\r\n");
  }
  else
  {
    printf("CreatReadProgressFile：创建文件成功\r\n");
    f_close(&s_fileProgressFile);
  }
}

/*********************************************************************************************************
* 函数名称：SaveReadProgress
* 函数功能：保存阅读进度
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、打开文件时文件变量FIL一定要是静态变量或动态内存分配的，否则会卡死，原因未知
*          1、FA_READ         ：指定对文件的读取权限。可以从文件中读取数据
*          2、FA_WRITE        ：指定对文件的写入访问权限。可以将数据写入该文件。与FA_READ组合以进行读写访问
*          3、FA_OPEN_EXISTING：打开一个文件。如果文件不存在，该函数将失败。（默认）
*          4、FA_CREATE_NEW   ：创建一个新文件。如果文件已存在，那么函数将失败并返回FR_EXIST
*          5、FA_CREATE_ALWAYS：创建一个新文件。如果文件已存在，那么它将被截断和覆盖
*          6、FA_OPEN_ALWAYS  ：打开文件（如果存在）。否则，将创建一个新文件
*          7、FA_OPEN_APPEND  ：与FA_OPEN_ALWAYS相同，只是读/写指针设置在文件末尾
*********************************************************************************************************/
void SaveReadProgress(void)
{
  static FIL  s_fileProgressFile;  //进度缓存文件
  static char s_arrStringBuf[256]; //字符串转换缓冲区
  FRESULT     result;              //文件操作返回变量
  u32         len;                 //字符串长度
  u32         writeNum;            //成功写入的数量

  //打开文件
  result = f_open(&s_fileProgressFile, "0:/book/progress/progress.txt", FA_OPEN_EXISTING | FA_WRITE);
  if(FR_OK != result)
  {
    printf("SaveReadProgress：打开文件失败\r\n");
    return;
  }

  //字符串转换
  sprintf(s_arrStringBuf, "阅读进度:%d/%d", GetBytePosition(), GetBookSize());

  //统计字符串长度
  len = 0;
  while(0 != s_arrStringBuf[len])
  {
    len++;
  }

  //将进度写入指定文件
  result = f_write(&s_fileProgressFile, s_arrStringBuf, len, &writeNum);
  if((FR_OK == result) && (writeNum == len))
  {
    printf("SaveReadProgress：保存进度成功\r\n");
  }
  else
  {
    printf("SaveReadProgress：保存进度失败\r\n");
  }

  //关闭文件
  f_close(&s_fileProgressFile);
}

/*********************************************************************************************************
* 函数名称：DeleteReadProgress
* 函数功能：删除阅读进度
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteReadProgress(void)
{
  f_unlink("0:/book/progress/progress.txt");
  printf("DeleteReadProgress：删除成功\r\n");
}
