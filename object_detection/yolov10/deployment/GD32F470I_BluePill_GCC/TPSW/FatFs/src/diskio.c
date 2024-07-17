/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"  /* FatFs lower layer API */
#include "sdcard.h"  
#include "FTL.h"
#include "RTC.h"

//#define ff_printf	printf
#define ff_printf(...)

#define SECTOR_SIZE 512 /* SD卡与Nand Flash扇区大小必须为512 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
  BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
  switch (pdrv)
  {
  
  //获取SD卡状态
  case FS_SD :
    return RES_OK;

  //获取Nand Flash状态
  case FS_NAND :
    return RES_OK;
  }
  
  return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
  BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
  switch (pdrv)
  {
    //初始化SD卡
    case FS_SD :
      if (sd_config() == SD_OK)
      {
        return RES_OK;
      }
      else
      {
        return RES_ERROR;
      }

    //初始化NAND Flash
    case FS_NAND :
      if(0 == InitFTL())
      {
        return RES_OK;
      }
      else
      {
        return RES_ERROR;
      }

    default :
      break;
  }
  return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
  BYTE pdrv,    /* Physical drive nmuber to identify the drive */
  BYTE *buff,   /* Data buffer to store read data */
  DWORD sector, /* Sector address in LBA */
  UINT count    /* Number of sectors to read */
)
{
  sd_error_enum status; //SD卡操作返回值

  //count不能等于0，否则返回参数错误
  if(0 == count)
  {
    return RES_PARERR;
  }

  switch (pdrv) 
  {

  //读取SD卡数据
  case FS_SD :
    //单扇区传输
    if (1 == count)
    {
      status = sd_block_read((uint32_t*)buff, sector << 9 , SECTOR_SIZE);
    }

    //多扇区传输
    else
    {
      status = sd_multiblocks_read((uint32_t*)buff, sector << 9 , SECTOR_SIZE, count);
    }

    //检验传输结果
    if (status != SD_OK)
    {
      return RES_ERROR;
    }

    //等待SD卡传输完成
    while(sd_transfer_state_get() != SD_NO_TRANSFER);
    return RES_OK;

  //读取Nand Flash数据
  case FS_NAND :
    if(0 == FTLReadSectors((u8*)buff, sector, 2048, count))
    {
      return RES_OK;
    }
    else
    {
      return RES_ERROR;
    }

  }
  return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
  BYTE pdrv,        /* Physical drive nmuber to identify the drive */
  const BYTE *buff, /* Data to be written */
  DWORD sector,     /* Sector address in LBA */
  UINT count        /* Number of sectors to write */
)
{
  sd_error_enum status; //SD卡操作返回值

  //count不能等于0，否则返回参数错误
  if(0 == count)
  {
    return RES_PARERR;
  }
  switch (pdrv) 
  {
    
  //将数据写入SD卡
  case FS_SD :
    //单扇区传输
    if (1 == count)
    {
      status = sd_block_write((uint32_t*)buff, sector << 9 ,SECTOR_SIZE);
    }

    //多扇区传输
    else
    {
      status = sd_multiblocks_write((uint32_t*)buff, sector << 9 ,SECTOR_SIZE, count);
    }

    //检验传输结果
    if (status != SD_OK)
    {
      return RES_ERROR;
    }

    //等待SD卡传输完成
    while(sd_transfer_state_get() != SD_NO_TRANSFER);
    return RES_OK;

  //将数据写入Nand Flash
  case FS_NAND :
    if(0 == FTLWriteSectors((u8*)buff, sector, 2048, count))
    {
      return RES_OK;
    }
    else
    {
      return RES_ERROR;
    }

  }
  return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
  BYTE pdrv,  /* Physical drive nmuber (0..) */
  BYTE cmd,   /* Control code */
  void *buff  /* Buffer to send/receive control data */
)
{
  sd_card_info_struct sdInfo; //SD卡信息

  switch (pdrv) {
  
  //SD卡控制
  case FS_SD :

    //获取SD卡信息
    sd_card_information_get(&sdInfo);
    switch(cmd)
    {
      //同步操作
      case CTRL_SYNC:
        return RES_OK; 
      
      //获取扇区大小
      case GET_SECTOR_SIZE:
        *(WORD*)buff = 512; 
        return RES_OK;

      //获得块大小
      case GET_BLOCK_SIZE:
        *(WORD*)buff = sdInfo.card_blocksize;
        return RES_OK;

      //获得扇区数量
      case GET_SECTOR_COUNT:
        *(DWORD*)buff = sdInfo.card_capacity / 512;
        return RES_OK;
      
      //非法参数
      default:
        return RES_PARERR;
    }

  //Nand Flash控制
  case FS_NAND :
    switch(cmd)
    {
      //同步操作
      case CTRL_SYNC:
        return RES_OK; 

      //获取扇区大小
      case GET_SECTOR_SIZE:
        *(WORD*)buff = 2048;
        return RES_OK;

      //获得块大小
      case GET_BLOCK_SIZE:
        *(WORD*)buff = (WORD)(NAND_BLOCK_SIZE);
        return RES_OK;

      //获取扇区数量
      case GET_SECTOR_COUNT:
        *(DWORD*)buff = NAND_BLOCK_COUNT * NAND_BLOCK_SIZE;
        return RES_OK;

      //擦除块
      case CTRL_ERASE_SECTOR:
        return RES_OK;

      //非法参数
      default:
        return RES_OK;
    }
  }
  return RES_PARERR;
}
#endif

/*
*********************************************************************************************************
*	函 数 名: get_fattime
*	功能说明: 获得系统时间，用于改写文件的创建和修改时间。客户可以自行移植和系统的RTC关联起来
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
DWORD get_fattime (void)
{
  // //如果有全局时钟，可按下面的格式进行时钟转换. 这个例子是2014-07-02 00:00:00
  // return ((DWORD)(2014 - 1980) << 25) //Year = 2013
  //      | ((DWORD)7 << 21)             //Month = 1
  //      | ((DWORD)2 << 16)             //Day_m = 1
  //      | ((DWORD)0 << 11)             //Hour = 0
  //      | ((DWORD)0 << 5)              //Min = 0
  //      | ((DWORD)0 >> 1);             //Sec = 0

  u32 year, month, date, week, hour, min, sec;

  //获取时间日期
  RTCGetDate(&year, &month, &date, &week);
  RTCGetTime(&hour, &min, &sec);

  //如果有全局时钟，可按下面的格式进行时钟转换. 这个例子是2014-07-02 00:00:00
  return ((DWORD)(year - 1980) << 25) //Year
       | ((DWORD)month << 21)         //Month
       | ((DWORD)date << 16)          //Day_m
       | ((DWORD)hour << 11)          //Hour
       | ((DWORD)min << 5)            //Min
       | ((DWORD)sec >> 1);           //Sec
}
