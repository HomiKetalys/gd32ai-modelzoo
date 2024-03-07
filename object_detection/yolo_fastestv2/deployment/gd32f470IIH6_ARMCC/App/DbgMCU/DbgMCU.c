/*********************************************************************************************************
* ģ������: DbgMCU.c
* ժ    Ҫ: �������
* ��ǰ�汾: 1.0.0
* ��    ��: SZLY(COPYRIGHT 2019 SZLY. All rights reserved.)
* �������: 2019��08��01��
* ��    ��: 
* ע    ��: 
**********************************************************************************************************
* ȡ���汾: 
* ��    ��: 
* �������: 
* �޸�����: 
* �޸��ļ�: 
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
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
*                                              �궨��
*********************************************************************************************************/
#define  PARA_NUM_MAX  10   //��������������
#define  FUNC_NUM_MAX  100  //�������Ӳ�����󳤶�

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
//��������ṹ��
typedef struct
{
  void* func;    //���Ժ���ָ��
  u8    paraNum; //��������
  char* name;    //��������
}StructDbgMCU;   //��������ṹ��

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
//���������б�
static StructDbgMCU s_arrDbgMCUProc[] =
{
  {PrintHelloWorld, 1, "PrintHelloWorld(num)"                           }, //PrintHelloWorld���Ժ���
  {ScreeShot,       0, "ScreeShot(void)"                                }, //��ͼ
  {RTCSet,          7, "RTCSet(year, month, date, week, hour, min, sec)"}, //����RTCʱ��
  {RTCPrintTime,    0, "RTCPrintTime(void)"                             }, //��ʾʱ��
  {RTCSetAlarm0,    3, "RTCSetAlarm0(hour, min, sec)"                   }, //��������0
  {RTCSetAlarm1,    3, "RTCSetAlarm1(hour, min, sec)"                   }, //��������1
  {PrintImTemp,     0, "PrintImTemp(void)"                              }, //��ӡCPU�ڲ��¶�
  {PrintSHT20,      0, "PrintSHT20(void)"                               }, //��ӡ��ʪ����
};

static int s_iFuncId;                                 //�����õ��ĺ���ID,��s_arrDbgMCUProc�������±�
static int s_arrPara[sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)][PARA_NUM_MAX]; //��ŵõ��ĺ�������

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void DbgShowHelp(void);         //��ʾ����
static void DbgMCUExe(void);           //����ִ�к���
static u8   DbgMCUAnalyza(u8 *data);   //���Է�������

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: DbgShowHelp
* ��������: ��ʾ����
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2019��08��01��
* ע    ��: 
*********************************************************************************************************/
static void DbgShowHelp(void)
{
  int i = 0;

  printf("\r\n");
  printf("\r\n");
  printf("------------------------DbgMCU V1.0.0-------------------------\r\n");
  printf("  DbgMCU�������������Ƽ����޹�˾�з��ĵ��Թ��ߣ�ͨ��������Ժ�\r\n");
  printf("����ĵ��ó����еĽӿڵ��Ժ�����ʹ�������ǳ����㡣\r\n");
  printf("  DbgMCU���֧��10�������ĵ��Ժ�����֧�ָ���������Ҳ֧������ո�\r\n");
  printf("ʹ��ʱ���ϸ��ո�ʽ���롣\r\n");
  printf("\r\n");
  printf("�����б�\r\n");
  for (i = 0; i < (sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)); i++)
  {
    printf("ID:%d, Name:%s\r\n", i, s_arrDbgMCUProc[i].name);
  }
  printf("\r\n");
  printf("ע�����\r\n");
  printf("1�����ϸ���ո�ʽ���루ID������1������2��...����ʾ����0:1\r\n");
  printf("2����֧�ֺ궨���������֧��ʮ���Ʋ�������ʹ��Ӣ���ַ�\r\n");
  printf("2�����ŷָ����������з�Ϊ���������\r\n");
  printf("4�����⡢Ѱ�⡢������WIFI���Ժ�����Ҫ��ʱ��������\r\n");
  printf("-------SZLY(COPYRIGHT 2018-2020 SZLY. All rights reserved.)--------\r\n");
}

/*********************************************************************************************************
* ��������: DbgMCUExe
* ��������: ��������ִ�к���
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2019��08��01��
* ע    ��:
*********************************************************************************************************/
static void DbgMCUExe(void)
{
  int i;
  int para[PARA_NUM_MAX];
  for (i = 0; i < s_arrDbgMCUProc[s_iFuncId].paraNum; i++)
  {
    para[i] = s_arrPara[s_iFuncId][i];
  }

  //��ӡִ����Ϣ
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

  //ִ�е��Ժ���
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
* ��������: DbgMCUAnalyza
* ��������: �������ַ�����������ȡ����ID�����
* �������: void
* �������: void
* �� �� ֵ: ok: 0-ָ�����1-�ɹ���2-��ʾ������3-��������
* ��������: 2019��08��01��
* ע    ��: ���ڶ�����Ĳ�����ȡ�����̬��
*********************************************************************************************************/
static u8 DbgMCUAnalyza(u8 *data)
{
  u8 i = 0;
  u8 paraNum = 0;
  u8 ok = 0;

  char numBuf[124] = { '\0' };
  int  numMark = 0;
  int  id = 0;

  //��ʾ����
  if(('h' == *(data + 0)) && ('e' == *(data + 1)) && ('l' == *(data + 2)) && ('p' == *(data + 3)))
  {
    ok = 2;
    return ok;
  }
  
  //����ð�Ų���ȡID
  i = 0;
  numMark = 0;
  while ((':' != *(data + i)) && (i < FUNC_NUM_MAX)) //���ִ��FUNC_NUM_MAX��
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
    return ok; //����ָ�����
  }
  else if(numMark > 0)
  {
    numBuf[numMark] = '\0';
    id = atoi(numBuf);
  }

  //����ID�ں���Χ֮��
  if (id < (sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)))
  {
    s_iFuncId = id;
  }
  else
  {
    ok = 0; 
    return ok;  //����ָ�����
  }

  //��ȡ����
  numMark = 0;
  while (i < FUNC_NUM_MAX)  //���ִ��FUNC_NUM_MAX��
  {
    //��������
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

    //����������ֹ��
    else if ('\0' == *(data + i))
    {
      numBuf[numMark] = '\0';
      if (numMark > 0)
      {
        s_arrPara[s_iFuncId][paraNum] = atoi(numBuf);
        paraNum++;
      }
      ok = 1; //�ɹ���ȡ����
      break;
    }

    //�����ں���Χ֮��
    else if(((*(data + i) >= '0') && (*(data + i) <= '9')) || '-' == *(data + i))
    {
      numBuf[numMark] = *(data + i);
      numMark++;
    }
    
    i++;
  }

  //��������
  if (paraNum != s_arrDbgMCUProc[s_iFuncId].paraNum)
  {
    ok = 3;
  }

  return ok;
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* ��������: InitDbgMCU
* ��������: ��ʼ���������
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2019��08��01��
* ע    ��:
*********************************************************************************************************/
void InitDbgMCU(void)
{
  int i = 0;
  int j = 0;

  InitCheckLineFeed();
  s_iFuncId = (sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)); //��ЧID

  for (i = 0; i < (sizeof(s_arrDbgMCUProc) / sizeof(StructDbgMCU)); i++)
  {
    for (j = 0; j < PARA_NUM_MAX; j++)
    {
      s_arrPara[i][j] = 0;
    }
  }
}

/*********************************************************************************************************
* ��������: DbgMCUScan
* ��������: ����ɨ�躯��
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2019��08��01��
* ע    ��:
*********************************************************************************************************/
void DbgMCUScan(void)             //����ɨ�躯��
{
  u8  recData;                    //���յ�����  
  i16 recCnt;                     //���յ������ݼ�����         
  u8  arrData[MAX_REC_DATA_CNT];  //���ݻ�����

  while (ReadUART0(&recData, 1))
  {
    if (1 == CheckLineFeed(recData))
    {
      recCnt = GetRecData(arrData);
      arrData[recCnt] = '\0';      //��ĩβ���������

      switch (DbgMCUAnalyza(arrData))
      {
      case 0:
        printf("ָ�����\r\n");
        break;
      case 1:
        DbgMCUExe();
        break;
      case 2:
        DbgShowHelp();
        break;
      case 3:
        printf("��������\r\n");
        break;
      default:
        break;
      }
    }
  }
}

/*********************************************************************************************************
* ��������: PrintHelloWorld
* ��������: ���Ժ���
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2019��08��01��
* ע    ��:
*********************************************************************************************************/
void PrintHelloWorld(int num)
{
  printf("Hello World! The number you sent is : %d\r\n", num);
}
