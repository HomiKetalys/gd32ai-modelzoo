/*********************************************************************************************************
* ģ�����ƣ�BookByteRead.h
* ժ    Ҫ�����ֽڴ��ı��ļ��ж�ȡ����ģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* ������ڣ�2021��07��01��
* ��    �ݣ�
* ע    �⣺SD����д�������ݿ����ʽ���У�ÿ�ζ�д512���ֽڣ�Ϊ�����SD����дЧ�ʣ���Ҫ��ר�ŵ�ģ�����SD����д
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
#include "BookByteRead.h"
#include "ff.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define BOOK_READ_BUF_SIZE (1024 * 5) //SD�����ʶ�ȡ����������Ҫ��4����������

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static FIL  s_fileBook;                       //�ı��ļ�
static char s_arrReadBuf[BOOK_READ_BUF_SIZE]; //��ȡ������
static u32  s_iByteRemain = 0;                //��������ʣ���ֽ�����Ϊ0ʱ��ʾ��Ҫ���ļ��ж�ȡ������
static u32  s_iEndFlag    = 0;                //�ļ���ȡ��ϱ�־λ��Ϊ1ʱ��ʾ�ļ��е������ѱ�ȫ����ȡ
static u32  s_iFilePos    = 0;                //�ļ���ȡλ��
static u32  s_iByteCnt    = 0;                //��ȡ�ֽڼ���
static u32  s_iBookSize   = 0;                //�鱾��С���ֽڣ�

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static u32 ReadData(void); //����һ������

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ReadData
* �������ܣ�����һ������
* ���������void
* ���������void
* �� �� ֵ��1-��ȡ�ɹ���0-��ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u32 ReadData(void)
{
  //�ļ���������ֵ
  FRESULT result;

  //ѭ������
  u32 i;

  //���ļ�
  result = f_open(&s_fileBook, "0:/book/���μ�.txt", FA_OPEN_EXISTING | FA_READ);
  if (result !=  FR_OK)
  {
    printf("BookByteRead: ��ָ���ļ�ʧ��\r\n");
    return 0;
  }

  //���ö�ȡλ�ã�Ҫȷ����ȡλ��Ϊ4�ı�������Ȼ�Ῠ����
  result = f_lseek(&s_fileBook, s_iFilePos * BOOK_READ_BUF_SIZE);
  if (result !=  FR_OK)
  {
    printf("BookByteRead: ���ö�ȡλ��ʧ��\r\n");
    return 0;
  }

  //�建����
  for(i = 0; i < BOOK_READ_BUF_SIZE; i++)
  {
    s_arrReadBuf[i] = 0;
  }

  //��ȡһ������
  result = f_read(&s_fileBook, s_arrReadBuf, BOOK_READ_BUF_SIZE, &s_iByteRemain);
  if (result !=  FR_OK)
  {
    printf("BookByteRead: ��ȡ����ʧ��\r\n");
    return 0;
  }

  //���¶�ȡλ��
  s_iFilePos = s_iFilePos + 1;

  //�ж��ǲ����ļ������һ������
  if(s_fileBook.fptr >= s_fileBook.fsize)
  {
    s_iEndFlag = 1;
  }

  //����ͼ���С
  s_iBookSize = s_fileBook.fsize;

  //�ر��ļ�
  result = f_close(&s_fileBook);
  if (result !=  FR_OK)
  {
    printf("BookByteRead: �ر�ָ���ļ�ʧ��\r\n");
    return 0;
  }

  return 1;
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ReadBookByte
* �������ܣ���ȡ1�ֽ�����
* ���������byte���������ݴ�������visi����ǰ�ַ���ʼ�����ж��ٸ������ַ���������ǰ�ַ���
* ���������void
* �� �� ֵ��1-��ȡ�ɹ���0-��ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 ReadBookByte(char* byte, u32* visi)
{
  u32 result;  //�ļ���������ֵ
  u32 visible; //�����ַ�ͳ��

  //��ǰ��������ʣ���ֽ���Ϊ0����Ҫ��ȡ��һ������
  if((0 == s_iByteRemain) && (0 == s_iEndFlag))
  {
    //��ȡһ������
    result = ReadData();
    if(0 == result)
    {
      return 0;
    }

    //��ȡ�ֽڼ�������
    s_iByteCnt = 0;
  }

  //��ǰ��������ʣ���ֽ���Ϊ0�������ļ���ȫ����ȡ���
  else if((0 == s_iByteRemain) && (1 == s_iEndFlag))
  {
    return 0;
  }

  //����ֽ�����
  *byte = s_arrReadBuf[s_iByteCnt];

  //�����ַ�ͳ��
  visible = 0;
  while(1)
  {
    //��������Ƿ�Խ��
    if((s_iByteCnt + visible + 1) >= BOOK_READ_BUF_SIZE)
    {
      break;
    }

    //���ҵ��˷ǿ����ַ�
    if(s_arrReadBuf[s_iByteCnt + visible + 1] <= ' ')
    {
      break;
    }
    visible++;
  }
  *visi = visible;

  //���¼���
  s_iByteCnt++;
  s_iByteRemain--;

  //��ȡ�ɹ�
  return 1;
}

/*********************************************************************************************************
* �������ƣ�GetBytePosition
* �������ܣ���ȡ��ǰ�ֽ����ı��ļ��е�λ��
* ���������void
* ���������void
* �� �� ֵ����ȡ��ǰ�ֽ����ı��ļ��е�λ��
* �������ڣ�2021��07��01��
* ע    �⣺��ǰ�ֽ�ָ���ǽ�Ҫ��ȡ���ֽ�
*********************************************************************************************************/
u32 GetBytePosition(void)
{
  //��δ��SD����ȡ�κ�����
  if(0 == s_iFilePos)
  {
    return 0;
  }
  else
  {
    return ((s_iFilePos - 1) * BOOK_READ_BUF_SIZE + s_iByteCnt);
  }
}

/*********************************************************************************************************
* �������ƣ�SetPosision
* �������ܣ����ö�ȡλ��
* ���������void
* ���������void
* �� �� ֵ��1-���óɹ���0-����ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺����Ķ�ȡλ����ָ�ļ��еĶ�ȡλ��
*********************************************************************************************************/
u32 SetPosision(u32 posision)
{
  //��ȡλ�ó����ļ���С
  if((posision >= s_iBookSize) && (0 != s_iBookSize))
  {
    return 0;
  }

  //������һ������
  s_iFilePos = posision / BOOK_READ_BUF_SIZE;
  if(0 == ReadData())
  {
    return 0;
  }

  //���¶�ȡ�ֽڼ���
  s_iByteCnt = posision % BOOK_READ_BUF_SIZE;

  //���»�����ʣ����
  s_iByteRemain = s_iByteRemain - s_iByteCnt;

  return 1;
}

/*********************************************************************************************************
* �������ƣ�GetBookSize
* �������ܣ���ȡ�鱾��С
* ���������void
* ���������void
* �� �� ֵ���鱾��С���ֽڣ�
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 GetBookSize(void)
{
  return s_iBookSize;
}