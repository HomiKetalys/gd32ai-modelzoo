/*********************************************************************************************************
* ģ�����ƣ�NandFlash.h
* ժ    Ҫ��NandFlash����ģ��
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
#ifndef _NAND_FLASH_H_
#define _NAND_FLASH_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
//NAND���붨��
#define EXMC_CMD_AREA       ((u32)(1 << 16)) //��������
#define EXMC_ADDR_AREA      ((u32)(1 << 17)) //��ַ����
#define EXMC_DATA_AREA      ((u32)0x0000000) //��������

//Nand Flash��ַ����
#define BANK1_NAND_ADDR     ((u32)0x70000000)
#define BANK_NAND_ADDR      BANK1_NAND_ADDR

//Nand Flash��������
#define NAND_CMD_AREA       (*(__IO u8 *)(BANK_NAND_ADDR | EXMC_CMD_AREA))  //д����
#define NAND_ADDR_AREA      (*(__IO u8 *)(BANK_NAND_ADDR | EXMC_ADDR_AREA)) //д��ַ
#define NAND_DATA_AREA      (*(__IO u8 *)(BANK_NAND_ADDR | EXMC_DATA_AREA)) //��д����

//Block��ҳ��ַ��λ����Block���תҳ�����Ҫ��
#define PAGE_BIT            (6)

//Nand Flash��������(GD9FU1G8F2AMG)
#define NAND_CMD_READ1_1ST         ((u8)0x00)
#define NAND_CMD_READ1_2ND         ((u8)0x30)
#define NAND_CMD_WRITE_1ST         ((u8)0x80)
#define NAND_CMD_WRITE_2ND         ((u8)0x10)
#define NAND_CMD_ERASE_1ST         ((u8)0x60)
#define NAND_CMD_ERASE_2ND         ((u8)0xD0)
#define NAND_CMD_READID            ((u8)0x90)
#define NAND_CMD_STATUS            ((u8)0x70)
#define NAND_CMD_LOCK_STATUS       ((u8)0x7A)
#define NAND_CMD_RESET             ((u8)0xFF)

//Nand Flash������������
#define NAND_MOVEDATA_CMD0         ((u8)0X00)
#define NAND_MOVEDATA_CMD1         ((u8)0X35)
#define NAND_MOVEDATA_CMD2         ((u8)0X85)
#define NAND_MOVEDATA_CMD3         ((u8)0X10)

//Nand Flash״̬λ����
#define NAND_BUSY                  ((u8)0x00)
#define NAND_ERROR                 ((u8)0x01)
#define NAND_READY                 ((u8)0x40)
#define NAND_TIMEOUT_ERROR         ((u8)0x80)

//Nand Flash�����趨
#define NAND_BLOCK_COUNT           ((u16)1024)   //Block����
#define NAND_ZONE_COUNT            ((u16)0x0001) //����һ��zone��plane��
#define NAND_ZONE_SIZE             ((u16)0x0400) //һ��zone����1024��Block
#define NAND_BLOCK_SIZE            ((u16)0x0040) //һ��Block����64��Page
#define NAND_PAGE_SIZE             ((u16)0x0800) //Page��Main����2 * 1024�ֽ�����
#define NAND_SPARE_AREA_SIZE       ((u16)0x0040) //Page��Spare����64���ֽ�
#define NAND_PAGE_TOTAL_SIZE       (NAND_PAGE_SIZE + NAND_SPARE_AREA_SIZE)
#define NAND_MAX_ADDRESS           (((NAND_ZONE_COUNT*NAND_ZONE_SIZE)*NAND_BLOCK_SIZE)*NAND_PAGE_SIZE)

//����Nand Flash��������ֵ
#define NAND_OK                    (0)
#define NAND_FAIL                  (1)

//NAND FLASH���������ʱ������38����ѭ����Լ��1us��������40����ѭ�����㣬����ȡ������Сֵȡ1
#define NAND_TADL_DELAY            (5)     //tADL�ȴ��ӳ٣�����100ns
#define NAND_TWHR_DELAY            (3)     //tWHR�ȴ��ӳ٣�����60ns
#define NAND_TWB_DELAY             (5)     //tWB�ȴ��ӳ٣����100ns
#define NAND_TAR_DELAY             (1)     //tAR�ȴ��ӳ٣���С15ns
#define NAND_TREA_DELAY            (1)     //tREA�ȴ���ʱ�����20ns
#define NAND_TRR_DELAY             (1)     //tRR��ʱ�ȴ�����С20ns
#define NAND_TPROG_DELAY           (700)   //tPROG�ȴ��ӳ�,����ֵ200us,�����Ҫ700us����΢����ʱ����ʵ��
#define NAND_TBERS_DELAY           (4)     //tBERS�ȴ��ӳ�,����ֵ2ms,�����Ҫ3ms���ɺ�����ʱ����ʵ��

/*********************************************************************************************************
*                                              �ṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
u32 InitNandFlash(void);                                               //��ʼ��NandFlash����ģ��
u32 NandReadID(void);                                                  //������ID
u32 NandReadStatus(void);                                              //������״̬
u32 NandWaitReady(void);                                               //�ȴ���������
u32 NandWritePage(u32 block, u32 page, u8* buf);                       //ҳд
u32 NandReadPage(u32 block, u32 page, u8* buf);                        //ҳ��
u32 NandWriteSpare(u32 block, u32 page, u32 column, u8* buf, u32 len); //дSpare��
u32 NandReadSpare(u32 block, u32 page, u32 column, u8* buf, u32 len);  //��Spare��
u32 NandEraseBlock(u32 blockNum);                                      //�����
u32 NandEraseChip(void);                                               //����ȫƬ
u32 NandReset(void);                                                   //��λ
u32 NandECCGetOE(u8 oe, u32 eccval);                                   //��ȡECC������λ/ż��λ
u32 NandECCCorrection(u8* data, u32 eccrd, u32 ecccl);                 //ECCУ������

//ҳ����
u32 NandCopyPageWithoutWrite(u32 sourceBlock, u32 sourcePage, u32 destBlock, u32 destPage);
u32 NandCopyPageWithWrite(u32 sourceBlock, u32 sourcePage, u32 destBlock, u32 destPage, u8* buf, u32 column, u32 len);

//�鿽��
u32 NandCopyBlockWithoutWrite(u32 sourceBlock, u32 destBlock, u32 startPage, u32 pageNum);
u32 NandCopyBlockWithWrite(u32 sourceBlock, u32 destBlock, u32 startPage, u32 column, u8* buf, u32 len);

//ҳУ��
u32 NandCheckPage(u32 block, u32 page, u32 value, u32 mode);
u32 NandPageFillValue(u32 block, u32 page, u32 value, u32 mode);

//ǿ�Ʋ���
u32 NandForceEraseBlock(u32 blockNum);
u32 NandForceEraseChip(void);
u32 NandMarkAllBadBlock(void);

#endif
