/*********************************************************************************************************
* 模块名称: DbgMCU.c
* 摘    要: 调试组件
* 当前版本: 1.0.0
* 作    者: SZLY(COPYRIGHT 2019 SZLY. All rights reserved.)
* 完成日期: 2019年08月01日
* 内    容: 
* 注    意: 
**********************************************************************************************************
* 取代版本: 
* 作    者: 
* 完成日期: 
* 修改内容: 
* 修改文件: 
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DbgMCU.h"
#include "DataType.h"
#include "stdlib.h"
#include "CheckLineFeed.h"
#include "stdio.h"
#include "UART0.h"
#include "DataType.h"
#include "BMPEncoder.h"
#include "RTC.h"
#include "InTemp.h"
#include "SHT20.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define  PARA_NUM_MAX  10   //函数参数最大个数
#define  FUNC_NUM_MAX  100  //函数名加参数最大长度

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
//调试任务结构体
typedef struct
{
  void* func;    //调试函数指针
  u8    paraNum; //参数个数
  char* name;    //函数名字
}StructDbgMCU;   //调试任务结构体

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
//调试任务列表
static StructDbgMCU s_arrDbgMCUProc[] =
{
  {PrintHelloWorld, 1, "PrintHelloWorld(num)"                           }, //PrintHelloWorld测试函数
  {ScreeShot,       0, "ScreeShot(void)"                                }, //截图
  {RTCSet,          7, "RTCSet(year, month, date, week, hour, min, sec)"}, //设置RTC时间
  {RTCPrintTime,    0, "RTCPrintTime(void)"                             }, //显示时间
  {RTCSetAlarm0,    3, "RTCSetAlarm0(hour, min, sec)"                   }, //设置闹钟0
  {RTCSetAlarm1,    3, "RTCSetAlarm1(hour, min, sec)"                   }, //设置闹钟1
  {PrintImTemp,     0, "PrintImTemp(void)"                              }, //打印CPU内部温度
  {PrintSHT20,      0, "PrintSHT20(void)"                               }, //打印温湿数据
};

static int s_iFuncId;                                 //分析得到的函数ID,即s_arrDbgMCUProc的数组下标
static int s_arrPara[sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)][PARA_NUM_MAX]; //存放得到的函数参数

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void DbgShowHelp(void);         //显示帮助
static void DbgMCUExe(void);           //调试执行函数
static u8   DbgMCUAnalyza(u8 *data);   //调试分析函数

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: DbgShowHelp
* 函数功能: 显示帮助
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2019年08月01日
* 注    意: 
*********************************************************************************************************/
static void DbgShowHelp(void)
{
  int i = 0;

  printf("\r\n");
  printf("\r\n");
  printf("------------------------DbgMCU V1.0.0-------------------------\r\n");
  printf("  DbgMCU是深圳市乐育科技有限公司研发的调试工具，通过它你可以很\r\n");
  printf("方便的调用程序中的接口调试函数，使用起来非常方便。\r\n");
  printf("  DbgMCU最多支持10个参数的调试函数，支持负数参数，也支持输入空格，\r\n");
  printf("使用时请严格按照格式输入。\r\n");
  printf("\r\n");
  printf("函数列表：\r\n");
  for (i = 0; i < (sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)); i++)
  {
    printf("ID:%d, Name:%s\r\n", i, s_arrDbgMCUProc[i].name);
  }
  printf("\r\n");
  printf("注意事项：\r\n");
  printf("1、请严格参照格式输入（ID：参数1，参数2，...），示例：0:1\r\n");
  printf("2、不支持宏定义参数，仅支持十进制参数，请使用英文字符\r\n");
  printf("2、逗号分隔参数，换行符为命令结束符\r\n");
  printf("4、红外、寻光、蓝牙、WIFI调试函数需要定时发送命令\r\n");
  printf("-------SZLY(COPYRIGHT 2018-2020 SZLY. All rights reserved.)--------\r\n");
}

/*********************************************************************************************************
* 函数名称: DbgMCUExe
* 函数功能: 调试助手执行函数
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2019年08月01日
* 注    意:
*********************************************************************************************************/
static void DbgMCUExe(void)
{
  int i;
  int para[PARA_NUM_MAX];
  for (i = 0; i < s_arrDbgMCUProc[s_iFuncId].paraNum; i++)
  {
    para[i] = s_arrPara[s_iFuncId][i];
  }

  //打印执行信息
  i = 0;
  printf("\r\n");
  while (('(' != *(s_arrDbgMCUProc[s_iFuncId].name + i)) && i < FUNC_NUM_MAX)
  {
    printf("%c", *(s_arrDbgMCUProc[s_iFuncId].name + i));
    i++;
  }
  printf("(");
  for (i = 0; i < s_arrDbgMCUProc[s_iFuncId].paraNum; i++)
  {
    printf("%d", para[i]);
    if (i != (s_arrDbgMCUProc[s_iFuncId].paraNum - 1))
    {
      printf(", ");
    }
  }
  printf(")\r\n");

  //执行调试函数
  switch (s_arrDbgMCUProc[s_iFuncId].paraNum)
  {
  case 0:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)();
    break;
  case 1:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0]);
    break;
  case 2:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1]);
    break;
  case 3:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1], para[2]);
    break;
  case 4:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1], para[2], para[3]);
    break;
  case 5:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1], para[2], para[3], para[4]);
    break;
  case 6:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1], para[2], para[3], para[4], para[5]);
    break;
  case 7:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1], para[2], para[3], para[4], para[5], 
    para[6]);
    break;
  case 8:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1], para[2], para[3], para[4], para[5], 
    para[6], para[7]);
    break;
  case 9:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1], para[2], para[3], para[4], para[5], 
    para[6], para[7], para[8]);
    break;
  case 10:
    (*(void(*)())s_arrDbgMCUProc[s_iFuncId].func)(para[0], para[1], para[2], para[3], para[4], para[5], 
    para[6], para[7], para[8], para[9]);
    break;
  default:
    break;
  }
}

/*********************************************************************************************************
* 函数名称: DbgMCUAnalyza
* 函数功能: 调试助手分析函数，获取函数ID与参数
* 输入参数: void
* 输出参数: void
* 返 回 值: ok: 0-指令错误，1-成功，2-显示帮助，3-参数错误
* 创建日期: 2019年08月01日
* 注    意: 对于多出来的参数采取不理睬态度
*********************************************************************************************************/
static u8 DbgMCUAnalyza(u8 *data)
{
  u8 i = 0;
  u8 paraNum = 0;
  u8 ok = 0;

  char numBuf[124] = { '\0' };
  int  numMark = 0;
  int  id = 0;

  //显示帮助
  if(('h' == *(data + 0)) && ('e' == *(data + 1)) && ('l' == *(data + 2)) && ('p' == *(data + 3)))
  {
    ok = 2;
    return ok;
  }
  
  //查找冒号并获取ID
  i = 0;
  numMark = 0;
  while ((':' != *(data + i)) && (i < FUNC_NUM_MAX)) //最多执行FUNC_NUM_MAX次
  {
    if ((*(data + i) >= '0') && (*(data + i) <= '9'))
    {
      numBuf[numMark] = *(data + i);
      numMark++;
    }
    i++;
  }
  if ((i == FUNC_NUM_MAX) || (0 == numMark))
  {
    ok = 0;
    return ok; //返回指令错误
  }
  else if(numMark > 0)
  {
    numBuf[numMark] = '\0';
    id = atoi(numBuf);
  }

  //函数ID在合理范围之内
  if (id < (sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)))
  {
    s_iFuncId = id;
  }
  else
  {
    ok = 0; 
    return ok;  //返回指令错误
  }

  //获取参数
  numMark = 0;
  while (i < FUNC_NUM_MAX)  //最多执行FUNC_NUM_MAX次
  {
    //遇见逗号
    if (',' == *(data + i))
    {
      numBuf[numMark] = '\0';
      if (numMark > 0)
      {
        s_arrPara[s_iFuncId][paraNum] = atoi(numBuf);
        paraNum++;
        numMark = 0;
      }
    }

    //遇见命令终止符
    else if ('\0' == *(data + i))
    {
      numBuf[numMark] = '\0';
      if (numMark > 0)
      {
        s_arrPara[s_iFuncId][paraNum] = atoi(numBuf);
        paraNum++;
      }
      ok = 1; //成功获取参数
      break;
    }

    //数据在合理范围之内
    else if(((*(data + i) >= '0') && (*(data + i) <= '9')) || '-' == *(data + i))
    {
      numBuf[numMark] = *(data + i);
      numMark++;
    }
    
    i++;
  }

  //参数错误
  if (paraNum != s_arrDbgMCUProc[s_iFuncId].paraNum)
  {
    ok = 3;
  }

  return ok;
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: InitDbgMCU
* 函数功能: 初始化调试组件
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2019年08月01日
* 注    意:
*********************************************************************************************************/
void InitDbgMCU(void)
{
  int i = 0;
  int j = 0;

  InitCheckLineFeed();
  s_iFuncId = (sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)); //无效ID

  for (i = 0; i < (sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)); i++)
  {
    for (j = 0; j < PARA_NUM_MAX; j++)
    {
      s_arrPara[i][j] = 0;
    }
  }
}

/*********************************************************************************************************
* 函数名称: DbgMCUScan
* 函数功能: 调试扫描函数
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2019年08月01日
* 注    意:
*********************************************************************************************************/
void DbgMCUScan(void)             //调试扫描函数
{
  u8  recData;                    //接收的数据  
  i16 recCnt;                     //接收到的数据计数器         
  u8  arrData[MAX_REC_DATA_CNT];  //数据缓冲区

  while (ReadUART0(&recData, 1))
  {
    if (1 == CheckLineFeed(recData))
    {
      recCnt = GetRecData(arrData);
      arrData[recCnt] = '\0';      //在末尾加入结束符

      switch (DbgMCUAnalyza(arrData))
      {
      case 0:
        printf("指令错误：\r\n");
        break;
      case 1:
        DbgMCUExe();
        break;
      case 2:
        DbgShowHelp();
        break;
      case 3:
        printf("参数错误：\r\n");
        break;
      default:
        break;
      }
    }
  }
}

/*********************************************************************************************************
* 函数名称: PrintHelloWorld
* 函数功能: 测试函数
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2019年08月01日
* 注    意:
*********************************************************************************************************/
void PrintHelloWorld(int num)
{
  printf("Hello World! The number you sent is : %d\r\n", num);
}
