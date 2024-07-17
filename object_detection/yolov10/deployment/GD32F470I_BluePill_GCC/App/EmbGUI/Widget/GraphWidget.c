/*********************************************************************************************************
* 模块名称：GraphWidget.c
* 摘    要：波形控件模块
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
#include "GraphWidget.h"
#include "TLILCD.h"
#include <math.h>

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
* 函数名称: CreateGraphWidget
* 函数功能: 新建波形绘制控件
* 输入参数: x0,y0：起始坐标（左上角），xSize：宽度，ySize：高度，graph：波形控件结构体
* 输出参数: void
* 返 回 值: void
* 创建日期: 2021年07月01日
* 注    意: graph需是静态结构体
*********************************************************************************************************/
void InitGraphWidgetStruct(StructGraphObj* graph)
{
  graph->backColor = 0x0000; //背景默认为黑色
  graph->penColor  = 0x07E0; //画笔颜色默认为绿色
  graph->curPos    = 0;      //当前绘制点的横坐标
  graph->WaveRatio = 1;      //设置波形比例
  graph->WaveSpeed = 1;      //每隔一个像素点画一个点
  graph->WavePos   = 0;      //默认竖直方向无偏移量
  graph->callBack  = NULL;   //绘图回调函数默认为空
  graph->hasCreate = 0;      //未被初始化
  graph->firstFlag = 0;      //未绘制第一个点
}

/*********************************************************************************************************
* 函数名称: CreateGraphWidget
* 函数功能: 新建波形绘制控件
* 输入参数: x0,y0：起始坐标（左上角），xSize：宽度，ySize：高度，graph：波形控件结构体
* 输出参数: void
* 返 回 值: void
* 创建日期: 2021年07月01日
* 注    意: graph需是静态结构体
*********************************************************************************************************/
void CreateGraphWidget(int x0, int y0, int xSize, int ySize, StructGraphObj* graph)
{
  int x1,y1;

  //计算横坐标终点
  x1 = x0 + xSize - 1;
	

  //计算纵坐标终点
  y1 = y0 + ySize - 1;

  graph->x0        = x0;    //记录起点坐标
  graph->y0        = y0;    //记录起点坐标
  graph->x1        = x1;    //记录终点坐标
  graph->y1        = y1;    //记录终点坐标

  //绘制背景
  LCDFillPixel(graph->x0, graph->y0, graph->x1, graph->y1, graph->backColor);

  //标记已经初始化
  graph->hasCreate = 1;

  //未开始绘图
  graph->startDraw = 0;

  //未绘制第一个点
  graph->firstFlag = 0;
}

/*********************************************************************************************************
* 函数名称: GraphWidgetAddData
* 函数功能: 波形增加点
* 输入参数: graph：控件结构体，data：点纵坐标（像素点）
* 输出参数: void
* 返 回 值: void
* 创建日期: 2021年07月01日
* 注    意: graph需是静态结构体
*********************************************************************************************************/
void GraphWidgetAddData(StructGraphObj* graph, unsigned int data)
{
  unsigned int x;  //用于计算下一个点的横坐标
  unsigned int y;  //用于计算下一个点的纵坐标

  //若还未初始化，则直接退出
  if(0 == graph->hasCreate || 0 == graph->startDraw)
  {
    return;
  }

  //波形幅值伸缩
  data = data / graph->WaveRatio;

  //上下移动
  data = data + graph->WavePos * (graph->y1 - graph->y0) / 8;

  //检查伸缩移动后的数据（数据范围为0~(graph->y1 - graph->y0)）
  if(data > (graph->y1 - graph->y0 - 1))
  {
    data = graph->y1 - graph->y0 - 1;
  }

  //上下翻转，因为画点函数原点在左上角，一般画图原点在左下角
  data = graph->y1 - data;

  //绘制第一个点
  if(0 == graph->firstFlag)
  {
    graph->firstFlag = 1;
    graph->xOld      = graph->x0;
    graph->yOld      = data;
  }
  else
  {
    //当前点的位置往右挪
    graph->curPos = graph->curPos + graph->WaveSpeed;
  }

  //当前位置超出绘图区域，循环储存
  if(graph->curPos > (graph->x1 - graph->x0 - 1))
  {
    graph->curPos = 0;
  }

  //设置前一个画点
  if(0 == graph->curPos)
  {
    graph->xOld = graph->x0;
    
    //删除原点处的点，否则原定处的点无法清除
    LCDDrawLine(graph->x0, graph->y0,graph->x0, graph->y1, graph->backColor);
  }

  //再次检查，防止数据越界
  if(data > graph->y1)
  {
    data = graph->y1;
  }

  if(data < (graph->y0))
  {
    data = graph->y0;
  }

  //计算终点位置
  x = graph->x0 + graph->curPos;
  y = data;

  //画线
  LCDDrawLine(graph->xOld, graph->yOld, x, y, graph->penColor);

  graph->xOld = x;
  graph->yOld = y;

  //画缺口，顺便清除之前预留下来的点
  LCDFillPixel(x + 1, graph->y0, x + 10 > graph->x1 ? graph->x1 :x + 10, graph->y1, graph->backColor);

  //执行回调函数
  if(NULL != graph->callBack)
  {
    graph->callBack();
  }
}

