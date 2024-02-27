/*********************************************************************************************************
* 模块名称：UART2.c
* 摘    要：串口模块，包括串口模块初始化，以及中断服务函数处理，以及读写串口函数实现
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日 
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
#include "UART2.h"
#include "gd32f470x_conf.h"
#include "Queue.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/
//串口发送状态
typedef enum
{
  UART_STATE_OFF, //串口未发送数据
  UART_STATE_ON,  //串口正在发送数据
  UART_STATE_MAX
}EnumUARTState;

/*********************************************************************************************************
*                                              内部变量定义
*********************************************************************************************************/    
static  StructCirQue s_structUARTSendCirQue;  //发送串口循环队列
static  StructCirQue s_structUARTRecCirQue;   //接收串口循环队列
static  u8  s_arrSendBuf[UART2_BUF_SIZE];     //发送串口循环队列的缓冲区
static  u8  s_arrRecBuf[UART2_BUF_SIZE];      //接收串口循环队列的缓冲区

static  u8  s_iUARTTxSts;                     //串口发送数据状态
          
/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static  void  InitUARTBuf(void);      //初始化串口缓冲区，包括发送缓冲区和接收缓冲区 
static  u8    WriteReceiveBuf(u8 d);  //将接收到的数据写入接收缓冲区
static  u8    ReadSendBuf(u8 *p);     //读取发送缓冲区中的数据
                                            
static  void  ConfigUART2(u32 bound);  //配置串口相关的参数，包括GPIO、RCC、USART和NVIC 
static  void  EnableUARTTx(void);     //使能串口发送，WriteUARTx中调用，每次发送数据之后需要调用                                      

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitUARTBuf
* 函数功能：初始化串口缓冲区，包括发送缓冲区和接收缓冲区  
* 输入参数：void
* 输出参数：void
* 返 回 值：void 
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static  void  InitUARTBuf(void)
{
  i32 i;

  for(i = 0; i < UART2_BUF_SIZE; i++)
  {
    s_arrSendBuf[i] = 0;
    s_arrRecBuf[i]  = 0;  
  }

  InitQueue(&s_structUARTSendCirQue, s_arrSendBuf, UART2_BUF_SIZE);
  InitQueue(&s_structUARTRecCirQue,  s_arrRecBuf,  UART2_BUF_SIZE);
}

/*********************************************************************************************************
* 函数名称：WriteReceiveBuf
* 函数功能：写数据到串口接收缓冲区 
* 输入参数：d，待写入串口接收缓冲区的数据
* 输出参数：void
* 返 回 值：写入数据成功标志，0-不成功，1-成功 
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static  u8  WriteReceiveBuf(u8 d)
{
  u8 ok = 0;  //写入数据成功标志，0-不成功，1-成功
                                                                    
  ok = EnQueue(&s_structUARTRecCirQue, &d, 1);   
                                                                    
  return ok;  //返回写入数据成功标志，0-不成功，1-成功 
}

/*********************************************************************************************************
* 函数名称：ReadSendBuf
* 函数功能：读取串口发送缓冲区中的数据 
* 输入参数：p，读出来的数据存放的首地址
* 输出参数：p，读出来的数据存放的首地址
* 返 回 值：读取数据成功标志，0-不成功，1-成功 
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static  u8  ReadSendBuf(u8 *p)
{
  u8 ok = 0;  //读取数据成功标志，0-不成功，1-成功
                                                                   
  ok = DeQueue(&s_structUARTSendCirQue, p, 1);  
                                                                   
  return ok;  //返回读取数据成功标志，0-不成功，1-成功 
}

/*********************************************************************************************************
* 函数名称：ConfigUART
* 函数功能：配置串口相关的参数，包括GPIO、RCC、USART和NVIC  
* 输入参数：bound，波特率
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static  void  ConfigUART2(u32 bound)
{
  nvic_irq_enable(USART2_IRQn, 0, 0);  //使能串口中断，设置优先级
  rcu_periph_clock_enable(RCU_GPIOC);  //使能GPIOA时钟


	
	gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_10);

	gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_11);

	
	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_10);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_11);

	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO_PIN_10);
	//gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);

  //配置USART的参数
  rcu_periph_clock_enable(RCU_USART2);              //使能串口时钟
  usart_deinit(USART2);                             //rcu配置恢复默认值
  usart_baudrate_set(USART2, bound);                //设置波特率
  usart_stop_bit_set(USART2,USART_STB_1BIT);        //设置停止位
  usart_word_length_set(USART2,USART_WL_8BIT);      //设置数据字长度
  usart_parity_config(USART2,USART_PM_NONE);        //设置奇偶校验位
  usart_receive_config(USART2, USART_RECEIVE_ENABLE);   //使能接收
  usart_transmit_config(USART2, USART_TRANSMIT_ENABLE); //使能发送
  usart_enable(USART2);  //使能串口

  usart_interrupt_enable(USART2, USART_INT_RBNE);  //使能接收缓冲区非空中断
  usart_interrupt_enable(USART2, USART_INT_TBE);   //使能发送缓冲区空中断
                                                                     
  s_iUARTTxSts = UART_STATE_OFF;  //串口发送数据状态设置为未发送数据
}

/*********************************************************************************************************
* 函数名称：EnableUARTTx
* 函数功能：使能串口发送，在WriteUARTx中调用，即每次发送数据之后需要调用这个函数来使能发送中断 
* 输入参数：void
* 输出参数：void
* 返 回 值：void 
* 创建日期：2018年01月01日
* 注    意：s_iUARTTxSts = UART_STATE_ON;这句话必须放在USART_ITConfig之前，否则会导致中断打开无法执行
*********************************************************************************************************/
static  void  EnableUARTTx(void)
{
  s_iUARTTxSts = UART_STATE_ON;                     //串口发送数据状态设置为正在发送数据

  usart_interrupt_enable(USART2, USART_INT_TBE);
}

/*********************************************************************************************************
* 函数名称：USART2_IRQHandler
* 函数功能：USART2中断服务函数 
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void USART2_IRQHandler(void)            
{
  u8  uData = 0;

  if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) != RESET)  //接收缓冲区非空中断
  {                                                         
    usart_interrupt_flag_clear(USART2, USART_INT_FLAG_RBNE);  //清除USART2中断挂起
    uData = usart_data_receive(USART2);  //将USART2接收到的数据保存到uData
                                                          
    WriteReceiveBuf(uData);  //将接收到的数据写入接收缓冲区                                 
  }                                                         
                                                            
  if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_ERR_ORERR) == SET) //溢出错误标志为1
  {                                                         
    usart_interrupt_flag_clear(USART2, USART_INT_FLAG_ERR_ORERR);       //清除溢出错误标志
    
    usart_data_receive(USART2);  //读取USART_DR 
  }                                                         
                                                           
  if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_TBE)!= RESET)  //发送缓冲区空中断
  {                       
    usart_interrupt_flag_clear(USART2, USART_INT_FLAG_TBE);        //清除发送中断标志
    //__NVIC_ClearPendingIRQ(USART2_IRQn);
                                   
    ReadSendBuf(&uData);                                 //读取发送缓冲区的数据到uData
                                                                    
    usart_data_transmit(USART2, uData);                  //将uData写入USART_DR
                                                                                           
    if(QueueEmpty(&s_structUARTSendCirQue))              //当发送缓冲区为空时
    {                                                               
      s_iUARTTxSts = UART_STATE_OFF;                     //串口发送数据状态设置为未发送数据       
      usart_interrupt_disable(USART2, USART_INT_TBE);    //关闭串口发送缓冲区空中断
    }
  }
} 

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitUART2
* 函数功能：初始化UART模块 
* 输入参数：bound,波特率
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void InitUART2(u32 bound)
{
  InitUARTBuf();        //初始化串口缓冲区，包括发送缓冲区和接收缓冲区  
                  
  ConfigUART2(bound);    //配置串口相关的参数，包括GPIO、RCC、USART和NVIC  
}

/*********************************************************************************************************
* 函数名称：WriteUART2
* 函数功能：写串口，即写数据到的串口发送缓冲区  
* 输入参数：pBuf，要写入数据的首地址，len，期望写入数据的个数
* 输出参数：void
* 返 回 值：成功写入数据的个数，不一定与形参len相等
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32  WriteUART2(u8 *pBuf, u32 len)
{
  u32 wLen = 0;  //实际写入数据的个数

  wLen = EnQueue(&s_structUARTSendCirQue, pBuf, len);

  if(wLen < UART2_BUF_SIZE)
  {
    if(s_iUARTTxSts == UART_STATE_OFF)
    {
      EnableUARTTx();
    }
  }

  return wLen;  //返回实际写入数据的个数
}

/*********************************************************************************************************
* 函数名称：ReadUART2
* 函数功能：读串口，即读取串口接收缓冲区中的数据  
* 输入参数：pBuf，读取的数据存放的首地址，len，期望读取数据的个数
* 输出参数：pBuf，读取的数据存放的首地址
* 返 回 值：成功读取数据的个数，不一定与形参len相等
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32  ReadUART2(u8 *pBuf, u32 len)
{
  u32 rLen = 0;  //实际读取数据长度

  rLen = DeQueue(&s_structUARTRecCirQue, pBuf, len);

  return rLen;  //返回实际读取数据的长度
}
    
