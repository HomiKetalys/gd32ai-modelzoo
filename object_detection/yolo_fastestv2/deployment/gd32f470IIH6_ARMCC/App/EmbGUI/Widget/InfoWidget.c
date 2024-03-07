/*********************************************************************************************************
* 模块名称：InfoWidget.c
* 摘    要：信息窗口控件
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
#include "InfoWidget.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitInfoWidgetStruct
* 函数功能：设置各项参数默认值
* 输入参数：widget：控件首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitInfoWidgetStruct(StructInfoWidget* widget)
{
  widget->x0 = 0;
  widget->y0 = 0;
  widget->width = 0;
  widget->height = 0;
  widget->topOff = 0;
  widget->bottomOff = 0;
  widget->leftOff = 0;
  widget->rightOff = 0;
  widget->textFont = GUI_FONT_ASCII_24;
  widget->textColor = GUI_COLOR_BLACK;
  widget->textHeight = 30;
  widget->background = NULL;
  widget->image = NULL;
  widget->imageType = GUI_IMAGE_TYPE_BMP;
  widget->defaultBackColor = GUI_COLOR_WHITE;
  widget->isExist = 0;
  widget->isShow = 0;
}

/*********************************************************************************************************
* 函数名称：CreateInfoWidget
* 函数功能：创建控件
* 输入参数：widget：控件首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateInfoWidget(StructInfoWidget* widget)
{
  //保存背景
  if(NULL != widget->background)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //计算显示区域大小
  widget->showX0 = widget->x0 + widget->leftOff;
  widget->showY0 = widget->y0 + widget->topOff;
  widget->showX1 = widget->x0 + widget->width - 1 - widget->rightOff;
  widget->showY1 = widget->y0 + widget->height - 1 - widget->bottomOff;
  widget->showWidth = widget->showX1 - widget->showX0 + 1;
  widget->showHeight = widget->showY1 - widget->showY0 + 1;

  //标记正在显示
  widget->isExist = 1;

  //标记未在显示内容
  widget->isShow = 0;
}

/*********************************************************************************************************
* 函数名称：DeleteInfoWidget
* 函数功能：删除控件
* 输入参数：widget：控件首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteInfoWidget(StructInfoWidget* widget)
{
  //还原背景
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //标记处于非活动状态
  widget->isExist = 0;

  //标记未在显示内容
  widget->isShow = 0;
}

/*********************************************************************************************************
* 函数名称：SetInfoWidgetShow
* 函数功能：设置信息框显示内容
* 输入参数：widget：控件首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void SetInfoWidgetShow(StructInfoWidget* widget, char* text)
{
  u32 x, y, x0, y0, x1, y1;
  u32 stringLen, lineLen, lineNum, stringLineNum, stringCnt, stringRemain, lineCnt, code;
  u32 i, j;

  //非活动状态下不显示
  if(0 == widget->isExist)
  {
    return;
  }

  //标记正在显示信息
  widget->isShow = 1;

  //还原背景
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //显示背景图片
  if(NULL != widget->image)
  {
    GUIDrawImage(widget->x0, widget->y0, (u32)widget->image, widget->imageType);
  }
  else
  {
    x1 = widget->x0 + widget->width - 1;
    y1 = widget->y0 + widget->height - 1;
    GUIFillColor(widget->x0, widget->y0, x1, y1, widget->defaultBackColor);
  }

  //统计字符串长度
  stringLen = 0;
  i = 0;
  while(0 != text[i++])
  {
    stringLen++;
  }

  //计算一行能显示的字符数
  lineLen = widget->showWidth / GetFontWidth(widget->textFont);

  //计算能显示的总行数
  lineNum = widget->showHeight / widget->textHeight;

  //计算字符串所占行数
  stringLineNum = stringLen / lineLen;
  if(0 != (stringLen % lineLen))
  {
    stringLineNum = stringLineNum + 1;
  }
  if(stringLineNum > lineNum)
  {
    stringLineNum = lineNum;
  }

  //计算显示起始坐标
  y0 = widget->showY0 + (widget->showHeight - widget->textHeight * stringLineNum) / 2;
  x0 = widget->showX0 + (widget->showWidth - lineLen * GetFontWidth(widget->textFont)) / 2;

  //循环显示字符
  stringCnt = 0;
  x = x0;
  y = y0;
  stringRemain = stringLen;
  for(i = 0; i < lineNum; i++)
  {
    //计算横坐标起点
    if(stringRemain >= lineLen)
    {
      x = x0;
    }
    else
    {
      x = widget->showX0 + (widget->showWidth - stringRemain * GetFontWidth(widget->textFont)) / 2;
    }

    //显示一行字符串
    lineCnt = 0;
    for(j = 0; j < lineLen; j++)
    {
      //还有字符未显示
      if(0 != text[stringCnt])
      {
        //英文字符
        if((u8)text[stringCnt] < 0x80)
        {
          if(lineCnt < lineLen)
          {
            code = text[stringCnt];
            GUIDrawChar(x, y, code, widget->textFont, NULL, widget->textColor, 1);
            x = x + GetFontWidth(widget->textFont);
            stringCnt = stringCnt + 1;
            lineCnt = lineCnt + 1;
            stringRemain = stringRemain - 1;
          }
          else
          {
            break;
          }
        }

        //中文字符
        else
        {
          if(lineCnt < (lineLen - 1))
          {
            code = 0;
            code = (text[stringCnt] << 8) | text[stringCnt + 1];
            GUIDrawChar(x, y, code, widget->textFont, NULL, widget->textColor, 1);
            x = x + 2 * GetFontWidth(widget->textFont);
            stringCnt = stringCnt + 2;
            lineCnt = lineCnt + 2;
            stringRemain = stringRemain - 2;
          }
          else
          {
            break;
          }
        }
      }

      //显示完成
      else
      {
        return;
      }
    }

    //更新横坐标
    y = y + widget->textHeight;
  }
}

/*********************************************************************************************************
* 函数名称：ScanInfoWidget
* 函数功能：控件扫描
* 输入参数：widget：控件首地址
* 输出参数：void
* 返 回 值：0-未被触摸到，1-又被触摸到
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 ScanInfoWidget(StructInfoWidget* widget)
{
  u32 tx, ty;         //触点坐标信息
  u32 x0, y0, x1, y1; //按键起点、终点坐标位置
  u32 i;              //循环变量

  //计算起点终点坐标
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width - 1;
  y1 = widget->y0 + widget->height - 1;

  //依次检查所有触点，查看是否有触点触碰到控件
  for(i = 0; i < GUI_TOUCH_NUM_MAX; i++)
  {
    if(1 == GUIGetTouch(&tx, &ty, i))
    {
      //判断是否按下
      if((tx >= x0) && (tx <= x1) && (ty >= y0) && (ty <= y1))
      {
        return 1;
      }
      else
      {
        continue;
      }
    }
    else
    {
      continue;
    }
  }

  //无按键触碰到控件，返回0
  return 0;
}

/*********************************************************************************************************
* 函数名称：SetInfoWidgetOff
* 函数功能：设置控件显示区域大小
* 输入参数：widget：控件首地址，off：偏移量
* 输出参数：void
* 返 回 值：0-未被触摸到，1-又被触摸到
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void SetInfoWidgetOff(StructInfoWidget* widget, u32 off)
{
  //设置偏移量
  widget->topOff = off;
  widget->bottomOff = off;
  widget->leftOff = off;
  widget->rightOff = off;
  
  //计算显示区域大小
  widget->showX0 = widget->x0 + widget->leftOff;
  widget->showY0 = widget->y0 + widget->topOff;
  widget->showX1 = widget->x0 + widget->width - 1 - widget->rightOff;
  widget->showY1 = widget->y0 + widget->height - 1 - widget->bottomOff;
  widget->showWidth = widget->showX1 - widget->showX0 + 1;
  widget->showHeight = widget->showY1 - widget->showY0 + 1;
}
