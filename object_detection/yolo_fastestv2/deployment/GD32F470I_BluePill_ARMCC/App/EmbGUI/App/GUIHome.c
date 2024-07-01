/*********************************************************************************************************
* 模块名称：GUIHome.c
* 摘    要：主界面
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
#include "GUIHome.h"
#include "TLILCD.h"
#include "GUIPlatform.h"
#include "BMP.h"
#include "Common.h"
#include "GUITop.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define ICON_IMAGE_WIDTH  (80)                //图标宽度
#define ICON_IMAGE_HEIGHT (80)                //图标高度
#define ICON_TEXT_FONT    (GUI_FONT_ASCII_24) //图标字体
#define ICON_TEXT_SPACE   (2)                 //字符串和图片的间隙
#define ICON_APP_SPAGE    ((480 - ICON_IMAGE_WIDTH * 4) / 5)

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
//主界面图标
typedef struct
{
  EnumGUIList     app;    //所属App
  u32             x0;     //坐标起点
  u32             y0;     //坐标起点
  u32             width;  //宽度
  u32             height; //高度
  StructBmpImage* image;  //图标图片，使用位图文件，没有定义时默认使用蓝色填充
  const char*     text;   //App标题，先默认使用24x24字体
}StructAppIcon;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructAppIcon  s_arrAppIcon[GUI_LIST_MAX - 1];  //APP图标集合
static StructBmpImage s_arrAppImage[GUI_LIST_MAX - 1]; //图标位图

//App标题
static const char* s_arrAppTitle[GUI_LIST_MAX - 1] = 
{
  "时钟",
  "RS232",
  "RS485",
  "CAN",
  "键盘",
  "文件",
  "以太网",
  "音乐",
  "录音机",
  "照相机",
  "阅读",
  "画布",
  "Wave",
  "温湿度",
  "呼吸灯",
  "OLED",
};

//App图标
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
*                                              内部函数声明
*********************************************************************************************************/
static void DisplayBackground(void);           //绘制背景
static void CreateAppIcon(StructAppIcon* app); //创建App图标
static void InitAllAppStruct(void);            //初始化所有App图标
static StructAppIcon* ScanAppIcon(void);       //扫描App图标

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：DisplayBackground
* 函数功能：绘制背景
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：调用之前要确保LCD方向为横屏
*********************************************************************************************************/
static void DisplayBackground(void)
{
  DisPlayBMPByIPA(0, 0, MAIN_WINDOWN_BACKGROUND_DIR);
}

/*********************************************************************************************************
* 函数名称：CreateAppIcon
* 函数功能：创建App图标
* 输入参数：app：App图标结构体地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void CreateAppIcon(StructAppIcon* app)
{
  u32 stringNum;      //字符串字符数
  u32 stringLen;      //字符串长度（像素点）
  u32 beginX, beginY; //字符串起点坐标
  u32 x0, y0, x1, y1;

  //显示图片
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

  //统计字符串总长度
  stringNum = 0;
  while(0 != app->text[stringNum])
  {
    stringNum++;
  }

  //校验字符串长度，App标题不能超过图片宽度
  if(stringNum > (app->width / GetFontWidth(ICON_TEXT_FONT)))
  {
    stringNum = app->width / GetFontWidth(ICON_TEXT_FONT);
  }

  //计算字符串长度（像素点）
  stringLen = stringNum * GetFontWidth(ICON_TEXT_FONT);
  
  //计算起始纵坐标
  beginY = app->y0 + ICON_IMAGE_HEIGHT + ICON_TEXT_SPACE;

  //计算起始横坐标
  beginX = app->x0 + (app->width - stringLen) / 2;

  //显示App标题
  GUIDrawTextLine(beginX, beginY, (u32)app->text, ICON_TEXT_FONT, NULL, GUI_COLOR_WHITE, 1, stringNum);
}

/*********************************************************************************************************
* 函数名称：InitAllAppStruct
* 函数功能：初始化所有App图标
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void InitAllAppStruct(void)
{
  u32 i, x, y;

  //初始化App位图参数
  for(i = 0; i < GUI_LIST_MAX - 1; i++)
  {
    s_arrAppImage[i].storageType = BMP_IN_FATFS;
    s_arrAppImage[i].alphaType = BMP_UES_ALPHA;
    s_arrAppImage[i].addr = (void*)s_arrImageAddr[i];
  }

  //初始化App图标
  x = ICON_APP_SPAGE;
  y = ICON_APP_SPAGE;
  for(i = 0; i < GUI_LIST_MAX - 1; i++)
  {
    //更新y轴
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

    //更新横坐标
    x = x + ICON_IMAGE_WIDTH + ICON_APP_SPAGE;
  }
}

/*********************************************************************************************************
* 函数名称：ScanAppIcon
* 函数功能：扫描App图标
* 输入参数：void
* 输出参数：void
* 返 回 值：按下的图标
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static StructAppIcon* ScanAppIcon(void)
{
  u32 i, x, y, x0, y0, x1, y1;

  //获取触屏坐标
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

  //无触点按下，直接返回
  else
  {
    return NULL;
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUIHome
* 函数功能：创建Home界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIHome(void)
{
  u32 i;

  //LCD竖屏显示
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //绘制背景
  DisplayBackground();

  //初始化图标参数
  InitAllAppStruct();

  //创建所有图标
  for(i = 0; i < GUI_LIST_MAX - 1; i++)
  {
    CreateAppIcon(&s_arrAppIcon[i]);
  }
}

/*********************************************************************************************************
* 函数名称：DeleteGUIHome
* 函数功能：删除主界面
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUIHome(void)
{

}

/*********************************************************************************************************
* 函数名称：GUIHomePoll
* 函数功能：主界面轮询
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIHomePoll(void)
{
  StructAppIcon* icon;
  icon = ScanAppIcon();
  if(NULL != icon)
  {
    printf("切换到：%s界面\r\n", icon->text);
    GUISwitch(icon->app);
  }
}
