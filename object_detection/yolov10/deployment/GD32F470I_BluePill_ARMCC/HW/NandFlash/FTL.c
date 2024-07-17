/*********************************************************************************************************
* ģ�����ƣ�FTL.c
* ժ    Ҫ��FTL���㷨
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
#include "FTL.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/
//FTL���ƽṹ��
typedef struct
{
  u32  pageTotalSize;         //ҳ�ܴ�С��main����spare���ܺ�
  u32  pageMainSize;          //ҳMain����С
  u32  pageSpareSize;         //ҳSpare����С
  u32  blockPageNum;          //�����ҳ����
  u32  planeBlockNum;         //Plane����Block����
  u32  blockTotalNum;         //�ܵĿ�����
  u32  goodBlockNum;          //�ÿ�����
  u32  validBlockNum;         //��Ч������(���ļ�ϵͳʹ�õĺÿ�����)
  u32  lut[NAND_BLOCK_COUNT]; //LUT�������߼���-�����ת��
  u32  sectorPerPage;         //ÿһҳ�к��ж��ٸ�����
}StructFTLDev;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructFTLDev s_structFTLDev;

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitFTL
* �������ܣ���ʼ��FTL���㷨
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ�������-ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 InitFTL(void)
{
  //��ʼ��FTL�豸�ṹ��
  s_structFTLDev.pageTotalSize = NAND_PAGE_TOTAL_SIZE;
  s_structFTLDev.pageMainSize  = NAND_PAGE_SIZE;
  s_structFTLDev.pageSpareSize = NAND_SPARE_AREA_SIZE;
  s_structFTLDev.blockPageNum  = NAND_BLOCK_SIZE;
  s_structFTLDev.planeBlockNum = NAND_ZONE_SIZE;
  s_structFTLDev.blockTotalNum = NAND_BLOCK_COUNT;

  //У��������С��Ҫ��֤������СΪ1ҳ����
  if(s_structFTLDev.pageMainSize != FTL_SECTOR_SIZE)
  {
    printf("InitFTL: error sector size\r\n");
    while(1);
  }
  s_structFTLDev.sectorPerPage = s_structFTLDev.pageMainSize / FTL_SECTOR_SIZE;

  //��ʼ��Nand Flash
  if(InitNandFlash())
  {
    return 1;
  }

  //����LUI��
  if(0 != CreateLUT())
  {
    //����LUT��ʧ�ܣ���Ҫ���³�ʼ��NandFlash
    printf("InitFTL: format nand flash...\r\n");
    if(FTLFormat())
    {
      printf("InitFTL: format failed!\r\n");
      return 2;
    }
  }

  //����LUT��ɹ������NandFlash����Ϣ
  printf("\r\n");
  printf("InitFTL: total block num:%d\r\n", s_structFTLDev.blockTotalNum);
  printf("InitFTL: good block num:%d\r\n", s_structFTLDev.goodBlockNum);
  printf("InitFTL: valid block num:%d\r\n", s_structFTLDev.validBlockNum);
  printf("\r\n");

  //��ʼ���ɹ�
  return 0;
}

/*********************************************************************************************************
* �������ƣ�FTL_BadBlockMark
* �������ܣ����ĳһ����Ϊ����
* ���������blockNum�����ţ���Χ��0~(blockTotalNum-1)
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void FTLBadBlockMark(u32 blockNum)
{
  //������mark������ֵ��OK��ֻҪ����0XFF
  //����дǰ4���ֽڣ�����FTL_FindUnusedBlock������黵��(����鱸����,������ٶ�)
  u32 mark = 0xAAAAAAAA;

  //�ڵ�һ��page��spare������һ���ֽ���������(ǰ4���ֽڶ�д)
  NandWriteSpare(blockNum, 0, 0, (u8*)&mark, 4);

  //�ڵڶ���page��spare������һ���ֽ���������(������,ǰ4���ֽڶ�д)
  NandWriteSpare(blockNum, 1, 0, (u8*)&mark, 4);
}

/*********************************************************************************************************
* �������ƣ�FTLCheckBlockFlag
* �������ܣ���黵���־λ
* ���������blockNum�����ţ���Χ��0~(blockTotalNum-1)
* ���������void
* �� �� ֵ��0-�ÿ飬����-����
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u32 FTLCheckBlockFlag(u32 blockNum)
{
  u8 flag = 0;

  //У���һҳ��־
  NandReadSpare(blockNum, 0, 0, (u8*)&flag, 1);
  if(0xFF == flag)
  {
    //У��ڶ�ҳ��־
    NandReadSpare(blockNum, 1, 0, (u8*)&flag, 1);
    if(0xFF == flag)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 2;
  }
}

/*********************************************************************************************************
* �������ƣ�FTLSetBlockUseFlag
* �������ܣ����ĳһ�����Ѿ�ʹ��
* ���������blockNum�����ţ���Χ��0~(blockTotalNum-1)
* ���������void
* �� �� ֵ��0-�ɹ�������-ʧ��
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u32 FTLSetBlockUseFlag(u32 blockNum)
{
  u8 flag = 0xCC;
  return NandWriteSpare(blockNum, 0, 1, (u8*)&flag, 1);
}

/*********************************************************************************************************
* �������ƣ�FTLLogicNumToPhysicalNum
* �������ܣ��߼����ת��Ϊ������
* ���������logicNum���߼�����
* ���������void
* �� �� ֵ���������
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u32 FTLLogicNumToPhysicalNum(u32 logicNum)
{
  if(logicNum > s_structFTLDev.blockTotalNum)
  {
    return INVALID_ADDR;
  }
  else
  {
    return s_structFTLDev.lut[logicNum];
  }
}

/*********************************************************************************************************
* �������ƣ�CreateLUT
* �������ܣ����´���LUT��
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ�������-ʧ��
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u32 CreateLUT(void)
{
  u32 i;        //ѭ������i
  u8  spare[6]; //Spareǰ6���ֽ�����
  u32 logicNum; //�߼�����
  
  //���LUT��
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    s_structFTLDev.lut[i] = INVALID_ADDR;
  }
  s_structFTLDev.goodBlockNum = 0;
  s_structFTLDev.validBlockNum = 0;

  //��ȡNandFlash�е�LUT��
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    //��ȡSpare��
    NandReadSpare(i, 0, 0, spare, 6);
    if(0xFF == spare[0])
    {
      NandReadSpare(i, 1, 0, spare, 1);
    }

    //�Ǻÿ�
    if(0xFF == spare[0])
    {
      //�õ��߼�����
      logicNum = ((u32)spare[5] << 24) | ((u32)spare[4] << 16) | ((u32)spare[3] << 8) | ((u32)spare[2] << 0);

      //�߼���ſ϶�С���ܵĿ�����
      if(logicNum < s_structFTLDev.blockTotalNum)
      {
        //����LUT��
        s_structFTLDev.lut[logicNum] = i;
      }

      //�ÿ����
      s_structFTLDev.goodBlockNum++;
    }
    else
    {
      printf("CreateLUT: bad block index:%d\r\n",i);
    }
  }

  //LUT��������Ժ�����Ч�����
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    if(s_structFTLDev.lut[i] < s_structFTLDev.blockTotalNum)
    {
      s_structFTLDev.validBlockNum++;
    }
  }

  //��Ч����С��100,������.��Ҫ���¸�ʽ��
  if(s_structFTLDev.validBlockNum < 100)
  {
    return 1;
  }

  //LUT�������
  return 0;
}

/*********************************************************************************************************
* �������ƣ�FTLFormat
* �������ܣ���ʽ��NAND �ؽ�LUT��
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ�������-ʧ��
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u32 FTLFormat(void)
{
  u32 i;         //ѭ������
  u32 ret;       //����ֵ
  u32 goodBlock; //���ڴ洢�ĺÿ�����
  u32 logicNum;  //�߼�����
  u8  spare[4];  //��дSpare��������

  //�������кÿ�
  s_structFTLDev.goodBlockNum = 0;
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    //���һ�����Ƿ�Ϊ����
    if(0 == FTLCheckBlockFlag(i))
    {
      //�����
      ret = NandEraseBlock(i);

      //����ʧ�ܣ�����Ϊ�ǻ���
      if(0 != ret)
      {
        printf("FTLFormat: back block: %d\r\n", i);
        FTLBadBlockMark(i);
      }
      else
      {
        //�ÿ�ͳ��
        s_structFTLDev.goodBlockNum++;
      }
    }
  }

  //У��ÿ�����������ÿ����������100����NAND Flash����
  printf("FTLFormat: good block num: %d\r\n", s_structFTLDev.goodBlockNum);
  if(s_structFTLDev.goodBlockNum < 100)
  {
    return 1;
  }

  //ʹ��90%�ĺÿ����ڴ洢����
  goodBlock = s_structFTLDev.goodBlockNum * 90 / 100;

  //�ںÿ��б�����߼�����Ϣ
  logicNum = 0;
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    if(0 == FTLCheckBlockFlag(i))
    {
      //д���߼�����
      spare[0] = (logicNum >> 0 ) & 0xFF;
      spare[1] = (logicNum >> 8 ) & 0xFF;
      spare[2] = (logicNum >> 16) & 0xFF;
      spare[3] = (logicNum >> 24) & 0xFF;
      NandWriteSpare(i, 0, 2, spare, 4);

      //�߼����ż�һ
      logicNum++;

      if(logicNum >= goodBlock)
      {
        break;
      }
    }
  }

  //�ؽ�LUT��
  if(CreateLUT())
  {
    return 2;
  }

  //��ʽ���ɹ�
  return 0;
}

/*********************************************************************************************************
* �������ƣ�FineUnuseBlock
* �������ܣ�����δʹ�õĿ飬ָ����ż
* ���������startBlock����ʼ���ţ�oddEven����ż״̬��1-������0-ż����
* ���������void
* �� �� ֵ��0-�ɹ�������-ʧ��
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u32 FTLFineUnuseBlock(u32 startBlock, u32 oddEven)
{
  u32 i;
  u8  byte[6];

  //�ɵ�ǰλ���������
  for(i = startBlock + 1; i < s_structFTLDev.blockTotalNum; i++)
  {
    if((i % 2) == oddEven)
    {
      //��ȡSpare��ǰ6���ֽ�
      NandReadSpare(i, 0, 0, byte, 6);

      //�ҵ�һ���տ죬���ر��
      if((0xFF == byte[0]) && (0xFF == byte[1]) && (0xFF == byte[2]) && (0xFF == byte[3]) && (0xFF == byte[4]) && (0xFF == byte[5]))
      {
        return i;
      }
    }
  }

  //��δ�ҵ������Դ�0��ʼ����
  for(i = 0; i < startBlock; i++)
  {
    if((i % 2) == oddEven)
    {
      //��ȡSpare��ǰ6���ֽ�
      NandReadSpare(i, 0, 0, byte, 6);

      //�ҵ�һ���տ죬���ر��
      if((0xFF == byte[0]) && (0xFF == byte[1]) && (0xFF == byte[2]) && (0xFF == byte[3]) && (0xFF == byte[4]) && (0xFF == byte[5]))
      {
        return i;
      }
    }
  }

  //���Ҳ������п飬������Чֵ
  return INVALID_ADDR;
}

/*********************************************************************************************************
* �������ƣ�FTLWriteSectors
* �������ܣ�д����(֧�ֶ�����д)��FATFS�ļ�ϵͳʹ��
* ���������pBuffer:���ݻ�����
*          sectorNo:��ʼ������
*          sectorSize:������С
*          sectorCount:Ҫд�����������
* ���������void
* �� �� ֵ��0-�ɹ�������-ʧ��
* �������ڣ�2018��01��01��
* ע    �⣺������С�̶�Ϊ2048�ֽ�
*********************************************************************************************************/
u32 FTLWriteSectors(u8 *pBuffer, u32 sectorNo, u32 sectorSize, u32 sectorCount)
{
  u32 logicBlock;  //�߼�����
  u32 phyBlock;    //�������
  u32 writePage;   //��ǰPage
  u32 writeColumn; //��ǰ��
  u32 ret;         //����ֵ
  u32 emptyBlock;  //���п���
  u32 copyLen;     //�鸴��ʱ��������
  u32 sectorCnt;   //д��������

  //У��������С
  if(FTL_SECTOR_SIZE != sectorSize)
  {
    return 1;
  }

  //ѭ��д����������
  sectorCnt = 0;
  while(sectorCnt < sectorCount)
  {
RETRY1_MARK:

    //����õ�λ��Block��Page�Լ�Column
    logicBlock  = (sectorNo / s_structFTLDev.sectorPerPage) / s_structFTLDev.blockPageNum;
    writePage   = (sectorNo / s_structFTLDev.sectorPerPage) % s_structFTLDev.blockPageNum;
    writeColumn = (sectorNo % s_structFTLDev.sectorPerPage) * sectorSize;

    //�߼�����ת�������
    phyBlock = FTLLogicNumToPhysicalNum(logicBlock);
    if(INVALID_ADDR == phyBlock)
    {
      //��������������ڴ棬д��ʧ��
      return 1;
    }

    //У�鵱ǰҳ�Ƿ��ѱ�д��
    ret = NandCheckPage(phyBlock, writePage, 0xFF, 0);

    //ҳδ��д���������ֱ��д��
    if(0 == ret)
    {
      //��ǵ�ǰ����ʹ��
      if(0 == writePage)
      {
        FTLSetBlockUseFlag(phyBlock);
      }

      //д������
      ret = NandWritePage(phyBlock, writePage, pBuffer);

      //д��ʧ�ܣ���ʾ�����˻��飬��Ҫ���������е�һ�����滻��ǰ��
      if(0 != ret)
      {
        //�ӵ�ǰ��������ҿ��п�
        emptyBlock = FTLFineUnuseBlock(phyBlock, (phyBlock % 2));
        if(INVALID_ADDR != emptyBlock)
        {
          //��������Bolck���ݣ�����Spare����
          ret = NandCopyBlockWithoutWrite(phyBlock, emptyBlock, 0, s_structFTLDev.blockPageNum);

          //�����ɹ�
          if(0 == ret)
          {
            //�޸�LUT��
            s_structFTLDev.lut[logicBlock] = emptyBlock;

            //��ǵ�ǰ��Ϊ����
            FTLBadBlockMark(phyBlock);
          }

          //����ʧ�ܣ���ʾ���BlockΪ����
          else
          {
            FTLBadBlockMark(emptyBlock);
          }

          //�ٴγ���д������
          goto RETRY1_MARK;
        }
        else
        {
          //��ǵ�ǰ��Ϊ����
          FTLBadBlockMark(phyBlock);

          //�Ҳ������п飬д�����
          return 1;
        }
      }

      //д��ɹ������¶�ȡ�����š�������ָ���Լ�д��������
      else
      {
        sectorNo = sectorNo + 1;
        pBuffer = pBuffer + sectorSize;
        sectorCnt = sectorCnt + 1;
      }
    }

    //��ǰҳ�б�д����������ݿ������������е�Block��������ͬʱд���µ�����
    else
    {
RETRY2_MARK:

      //�ӵ�ǰ��������ҿ��п�
      emptyBlock = FTLFineUnuseBlock(phyBlock, (phyBlock % 2));
      if(INVALID_ADDR != emptyBlock)
      {
        //����������д���������
        copyLen = s_structFTLDev.blockPageNum * s_structFTLDev.pageMainSize - s_structFTLDev.pageMainSize * writePage - writeColumn;

        //��Ҫ������������С�����д��������������ʵ��д���Ϊ׼
        if(copyLen >= ((sectorCount - sectorCnt) * sectorSize))
        {
          copyLen = (sectorCount - sectorCnt) * sectorSize;
        }

        //����ǰ�����ݿ����������������ݣ���д������
        ret = NandCopyBlockWithWrite(phyBlock, emptyBlock, writePage, writeColumn, pBuffer, copyLen);

        //�����ɹ�
        if(0 == ret)
        {
          //�޸�LUT��
          s_structFTLDev.lut[logicBlock] = emptyBlock;

          //������ǰ�飬ʹ֮��ɱ�������
          ret = NandEraseBlock(phyBlock);
          if(0 != ret)
          {
            //����ʧ�ܣ����Ϊ����
            FTLBadBlockMark(phyBlock);
          }

          //���¶�ȡ�����š�������ָ���Լ�д��������
          sectorNo = sectorNo + copyLen / sectorSize;
          pBuffer = pBuffer + copyLen;
          sectorCnt = sectorCnt + copyLen / sectorSize;
        }

        //����ʧ�ܣ���ʾ���BlockΪ����
        else
        {
          //���Ϊ����
          FTLBadBlockMark(emptyBlock);

          //�ٴγ���
          goto RETRY2_MARK;
        }
      }
      else
      {
        //�Ҳ������п飬NandFlash�ڴ� �������꣬д��ʧ��
        return 1;
      }
    }
  }

  return 0;
}

/*********************************************************************************************************
* �������ƣ�FTL_ReadSectors
* �������ܣ�������(֧�ֶ�������)��FATFS�ļ�ϵͳʹ��
* ���������pBuffer:���ݻ�����
*          sectorNo:��ʼ������
*          sectorSize:������С
*          sectorCount:Ҫ��ȡ����������
* ���������void
* �� �� ֵ��0-�ɹ�������-ʧ��
* �������ڣ�2018��01��01��
* ע    �⣺������С�̶�Ϊ2048�ֽ�
*********************************************************************************************************/
u32 FTLReadSectors(u8 *pBuffer, u32 sectorNo, u32 sectorSize, u32 sectorCount)
{
  u32 i;          //ѭ������
  u32 readBlock;  //��ǰBlock
  u32 readPage;   //��ǰPage
  //u32 readColumn; //��ǰ��
  u32 ret;        //����ֵ

  //У��������С
  if(FTL_SECTOR_SIZE != sectorSize)
  {
    return 1;
  }

  for(i = 0; i < sectorCount; i++)
  {
    //����õ�λ��Block��Page�Լ�Column
    readBlock  = (sectorNo / s_structFTLDev.sectorPerPage) / s_structFTLDev.blockPageNum;
    readPage   = (sectorNo / s_structFTLDev.sectorPerPage) % s_structFTLDev.blockPageNum;
    //readColumn = (sectorNo % s_structFTLDev.sectorPerPage) * sectorSize;

    //�߼�����ת�������
    readBlock = FTLLogicNumToPhysicalNum(readBlock);
    if(INVALID_ADDR == readBlock)
    {
      //��������������ڴ棬��ȡʧ��
      return 1;
    }

    //��ȡ����
    ret = NandReadPage(readBlock, readPage, pBuffer);
    if(0 != ret)
    {
      //��ȡʧ�ܣ��ٴγ��Զ�ȡ
      ret = NandReadPage(readBlock, readPage, pBuffer);
      if(0 != ret)
      {
        //�������λ����ɹ�����ȡʧ��
        return 1;
      }
    }

    //���¶�ȡ�����źͻ�����ָ��
    sectorNo = sectorNo + 1;
    pBuffer = pBuffer + sectorSize;
  }

  return 0;
}
