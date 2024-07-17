/*********************************************************************************************************
* ģ�����ƣ�GUIReader.c
* ժ    Ҫ��ͼ��ģ��
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
*                                              �궨��
*********************************************************************************************************/
#define BOOK_X0     (6)    //��ʾ����x0
#define BOOK_Y0     (62)   //��ʾ����y0
#define BOOK_X1     (474)  //��ʾ����x1
#define BOOK_Y1     (734)  //��ʾ����y1
#define FONT_WIDTH  (12)   //��ʾ������
#define FONT_HEIGHT (30)   //��ʾ����߶�
#define MAX_LINE_NUM ((BOOK_Y1 - BOOK_Y0) / FONT_HEIGHT) //�������

//�����һҳ����
#define MAX_PREV_PAGE (64)

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
typedef enum
{
  GUI_BUTTON_PREVIOUS = 0, //ǰһҳ���������������Ǵ�0
  GUI_BUTTON_NEXT,         //��һҳ����
  GUI_BUTTON_MAX,          //��������
  GUI_BUTTON_NONE,         //��Ч����
}EnumGUIButtons;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX]; //��������
static EnumGUIButtons     s_enumButtonPress;           //��������Ǹ���������

static u32          s_iBackColor;    //������ɫ
static char         s_iEndFlag = 0;  //�ı���ʾ��ȫ��־λ
static char         s_iLastChar = 0; //��һ��δ��ʾ�������ַ�
static u32          s_arrPrevPosition[MAX_PREV_PAGE]; //��һҳ��ʼλ��(0xFFFFFFFF��ʾ������)

//����
static char* s_pBookName = "���μ�";

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ButtonPressCallback(void* button);  //�����ص�����
static EnumGUIButtons ScanGUIButton(void);      //����ɨ��
static void NewPage(void);      //��ʾ�µ�һҳ
static void PreviousPage(void); //��ʾ��һҳ
static void NextPage(void);     //��һҳ

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ButtonPressCallback
* �������ܣ������ص�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�ScanGUIButton
* �������ܣ�����ɨ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺ÿ��20����ִ��һ��
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
* �������ƣ�NewPage
* �������ܣ���ʾ�µ�һҳ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void NewPage(void)
{
  u8  newchar;                //��ǰ�ֽ�
  u16 cnChar;                 //���ķ���
  u32 lineCnt;                //��ǰ��λ��
  u32 rowCnt;                 //��ǰ��λ��
  u32 x;                      //�ַ���ʾ������
  u32 y;                      //�ַ���ʾ������
  u32 visibleLen;             //��ǰ�ַ���ʼ�����ж��ٸ������ַ�
  u32 newParaFlag;            //�¶α�־λ�����ڷǶ��俪ͷ���в���ʾ��

  //�����ʾ
  GUIFillColor(BOOK_X0, BOOK_Y0, BOOK_X1, BOOK_Y1, s_iBackColor);

  //��ʾ��һ��δ��ӡ�������ַ�
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

  //��ʾһ��ҳ����
  newchar = 0;
  newParaFlag = 0;
  s_iLastChar = 0;
  while(1)
  {
    cnChar = 0;

    //�ӻ������ж�ȡ1�ֽ�����
    if(0 == ReadBookByte((char*)&newchar, &visibleLen))
    {
      s_iEndFlag = 1;
      return;
    }

    //�س����з���
    if('\r' == newchar)
    {
      rowCnt = 0;
    }

    //����
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

    //Ӣ��
    if((newchar >= ' ') && (newchar <= '~'))
    {
      //��鵱ǰ���Ƿ��㹻��ʾ��������
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

      //���¶����пո���ʾ
      if((0 == rowCnt) && (0 == newParaFlag) && (' ' == newchar))
      {
        continue;
      }

      x = BOOK_X0 + FONT_WIDTH * rowCnt;
      y = BOOK_Y0 + FONT_HEIGHT * lineCnt;
      GUIDrawChar(x, y, newchar, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1);
    }

    //����
    else if(newchar >= 0x81)
    {
      //������ֽ�
      cnChar = newchar << 8;

      //�ӻ������ж�ȡ1�ֽ�����
      if(0 == ReadBookByte((char*)&newchar, &visibleLen))
      {
        s_iEndFlag = 1;
        return;
      }

      //��ϳ�һ�������ĺ�������
      cnChar = cnChar | newchar;

      //��ʾ
      x = BOOK_X0 + FONT_WIDTH * rowCnt;
      y = BOOK_Y0 + FONT_HEIGHT * lineCnt;
      //ShowCNChar(x, y, cnChar, GUI_COLOR_BLACK, NULL, 24, 1);
      GUIDrawChar(x, y, cnChar, GUI_FONT_ASCII_24, NULL, GUI_COLOR_BLACK, 1);
    }

    //�����м���������ռ2�У�
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

    //�����м���
    if(0 == rowCnt)
    {
      lineCnt = lineCnt + 1;
      if(lineCnt >= MAX_LINE_NUM)
      {
        return;
      }
    }

    //����¶α�־λ
    newParaFlag = 0;
  }
}

/*********************************************************************************************************
* �������ƣ�PreviousPage
* �������ܣ���һҳ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void PreviousPage(void)
{
  u32  i; //ѭ������

  //ˢ����һҳλ������
  if(0xFFFFFFFF != s_arrPrevPosition[MAX_PREV_PAGE - 2])
  {
    for(i = (MAX_PREV_PAGE - 1); i > 0; i--)
    {
      s_arrPrevPosition[i] = s_arrPrevPosition[i - 1];
    }
    s_arrPrevPosition[0] = 0xFFFFFFFF;
  }

  //��һҳ������
  if(0xFFFFFFFF != s_arrPrevPosition[MAX_PREV_PAGE - 1])
  {
    //�ɹ����ö�дλ��
    if(1 == SetPosision(s_arrPrevPosition[MAX_PREV_PAGE - 1]))
    {
      s_iEndFlag = 0;
      s_iLastChar = 0;

      //ˢ���µ�һҳ
      NewPage();
    }
  }
}

/*********************************************************************************************************
* �������ƣ�NextPage
* �������ܣ���һҳ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void NextPage(void)
{
  u32  i; //ѭ������

  //�ı���ȫ����ʾ��ֱ���˳�
  if(1 == s_iEndFlag)
  {
    return;
  }

  //������һҳλ��
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

  //ˢ���µ�һҳ
  NewPage();
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CreateGUIReader
* �������ܣ�����ͼ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateGUIReader(void)
{
  static StructBmpImage s_structKeyPreviousPressImage;
  static StructBmpImage s_structKeyPreviousReleaseImage;
  static StructBmpImage s_structKeyNextPressImage;
  static StructBmpImage s_structKeyNextReleaseImage;

  u32 len, max, total, begin;

  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_VERTICAL);

  //��ʾ����ͼƬ
  DisPlayBMPByIPA(0, 0, READER_BACKGROUND_DIR);

  //����ް�������
  s_enumButtonPress = GUI_BUTTON_NONE;

  //��ʼ��λͼ
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

  //����ǰһҳ��ť
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

  //������һҳ��ť
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

  //������������
  len = 0;
  while((0 != s_pBookName[len]) && ('\r' != s_pBookName[len]) && ('\n' != s_pBookName[len]))
  {
    len++;
  }

  //������Ļһ�������ʾ���ٸ��ַ�
  max = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer] / 12;

  //��������ص�����
  total = 12 * len;

  //������ʼ��ʾ���ص�
  begin = (g_structTLILCDDev.width[g_structTLILCDDev.currentLayer] - total) / 2;

  //��ʾ����
  GUIDrawTextLine(begin, 15, (u32)s_pBookName, GUI_FONT_ASCII_24, NULL, GUI_COLOR_WHITE, 1, max);

  //��ȡ�Ķ�������ɫ��ʹ�ô�ɫ������
  s_iBackColor = GUIReadPoint((BOOK_X0 + BOOK_X1) / 2, (BOOK_Y0 + BOOK_Y1) / 2);

  //��ˢ��һҳ����
  NextPage();
}

/*********************************************************************************************************
* �������ƣ�DeleteGUIReader
* �������ܣ�ɾ��ͼ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteGUIReader(void)
{

}

/*********************************************************************************************************
* �������ƣ�GUIReaderPoll
* �������ܣ�ͼ����ѯ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIReaderPoll(void)
{
  EnumGUIButtons button;

  //����ɨ��
  button = ScanGUIButton();
  if(GUI_BUTTON_NONE != button)
  {
    //ǰһҳ
    if(GUI_BUTTON_PREVIOUS == button)
    {
      PreviousPage();
    }

    //��һҳ
    else if(GUI_BUTTON_NEXT == button)
    {
      NextPage();
    }
  }
}

/*********************************************************************************************************
* �������ƣ�CreatReadProgressFile
* �������ܣ����������Ķ������ļ�
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1�����ļ�ʱ�ļ�����FILһ��Ҫ�Ǿ�̬������̬�ڴ����ģ�����Ῠ����ԭ��δ֪
*          1��FA_READ         ��ָ�����ļ��Ķ�ȡȨ�ޡ����Դ��ļ��ж�ȡ����
*          2��FA_WRITE        ��ָ�����ļ���д�����Ȩ�ޡ����Խ�����д����ļ�����FA_READ����Խ��ж�д����
*          3��FA_OPEN_EXISTING����һ���ļ�������ļ������ڣ��ú�����ʧ�ܡ���Ĭ�ϣ�
*          4��FA_CREATE_NEW   ������һ�����ļ�������ļ��Ѵ��ڣ���ô������ʧ�ܲ�����FR_EXIST
*          5��FA_CREATE_ALWAYS������һ�����ļ�������ļ��Ѵ��ڣ���ô�������ضϺ͸���
*          6��FA_OPEN_ALWAYS  �����ļ���������ڣ������򣬽�����һ�����ļ�
*          7��FA_OPEN_APPEND  ����FA_OPEN_ALWAYS��ͬ��ֻ�Ƕ�/дָ���������ļ�ĩβ
*********************************************************************************************************/
void CreatReadProgressFile(void)
{
  static FIL s_fileProgressFile; //���Ȼ����ļ�
  DIR        progressDir;        //Ŀ��·��
  FRESULT    result;             //�ļ��������ر���
  
  //У����Ȼ���·���Ƿ���ڣ����������򴴽���·��
  result = f_opendir(&progressDir,"0:/book/progress");
  if(FR_NO_PATH == result)
  {
    f_mkdir("0:/book/progress");
  }
  else
  {
    f_closedir(&progressDir);
  }

  //����ļ��Ƿ���ڣ��粻�����򴴽�
  result = f_open(&s_fileProgressFile, "0:/book/progress/progress.txt", FA_CREATE_NEW | FA_READ);
  if(FR_OK != result)
  {
    printf("CreatReadProgressFile���ļ��Ѵ���\r\n");
  }
  else
  {
    printf("CreatReadProgressFile�������ļ��ɹ�\r\n");
    f_close(&s_fileProgressFile);
  }
}

/*********************************************************************************************************
* �������ƣ�SaveReadProgress
* �������ܣ������Ķ�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1�����ļ�ʱ�ļ�����FILһ��Ҫ�Ǿ�̬������̬�ڴ����ģ�����Ῠ����ԭ��δ֪
*          1��FA_READ         ��ָ�����ļ��Ķ�ȡȨ�ޡ����Դ��ļ��ж�ȡ����
*          2��FA_WRITE        ��ָ�����ļ���д�����Ȩ�ޡ����Խ�����д����ļ�����FA_READ����Խ��ж�д����
*          3��FA_OPEN_EXISTING����һ���ļ�������ļ������ڣ��ú�����ʧ�ܡ���Ĭ�ϣ�
*          4��FA_CREATE_NEW   ������һ�����ļ�������ļ��Ѵ��ڣ���ô������ʧ�ܲ�����FR_EXIST
*          5��FA_CREATE_ALWAYS������һ�����ļ�������ļ��Ѵ��ڣ���ô�������ضϺ͸���
*          6��FA_OPEN_ALWAYS  �����ļ���������ڣ������򣬽�����һ�����ļ�
*          7��FA_OPEN_APPEND  ����FA_OPEN_ALWAYS��ͬ��ֻ�Ƕ�/дָ���������ļ�ĩβ
*********************************************************************************************************/
void SaveReadProgress(void)
{
  static FIL  s_fileProgressFile;  //���Ȼ����ļ�
  static char s_arrStringBuf[256]; //�ַ���ת��������
  FRESULT     result;              //�ļ��������ر���
  u32         len;                 //�ַ�������
  u32         writeNum;            //�ɹ�д�������

  //���ļ�
  result = f_open(&s_fileProgressFile, "0:/book/progress/progress.txt", FA_OPEN_EXISTING | FA_WRITE);
  if(FR_OK != result)
  {
    printf("SaveReadProgress�����ļ�ʧ��\r\n");
    return;
  }

  //�ַ���ת��
  sprintf(s_arrStringBuf, "�Ķ�����:%d/%d", GetBytePosition(), GetBookSize());

  //ͳ���ַ�������
  len = 0;
  while(0 != s_arrStringBuf[len])
  {
    len++;
  }

  //������д��ָ���ļ�
  result = f_write(&s_fileProgressFile, s_arrStringBuf, len, &writeNum);
  if((FR_OK == result) && (writeNum == len))
  {
    printf("SaveReadProgress��������ȳɹ�\r\n");
  }
  else
  {
    printf("SaveReadProgress���������ʧ��\r\n");
  }

  //�ر��ļ�
  f_close(&s_fileProgressFile);
}

/*********************************************************************************************************
* �������ƣ�DeleteReadProgress
* �������ܣ�ɾ���Ķ�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteReadProgress(void)
{
  f_unlink("0:/book/progress/progress.txt");
  printf("DeleteReadProgress��ɾ���ɹ�\r\n");
}
