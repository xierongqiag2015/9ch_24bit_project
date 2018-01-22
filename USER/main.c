/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   TCP Client例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Bsp/led/bsp_led.h" 
#include "Bsp/usart/bsp_debug_usart.h"
#include "Bsp/systick/bsp_SysTick.h"
#include "Bsp/key/bsp_key.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/tcp_impl.h"
#include "netconf.h"
#include "tcp_echoclient.h"
#include "udp_echoclient.h"
#include "stm32f4x7_phy.h"


#include "./adc/bsp_adc.h"
#include "delay.h"
#include "./lcd/bsp_ili9806g_lcd.h"
#include "ff.h"
#include "./RTC/bsp_rtc.h"
#include "./485/bsp_485.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint8_t EthLinkStatus;
__IO uint64_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
__IO uint32_t KEEPALIVETime = 0;
__IO uint64_t KEEPCONNECTTime = 0;
/* Private function prototypes -----------------------------------------------*/
static void TIM3_Config(uint16_t period,uint16_t prescaler);
static void TIM4_Config(uint16_t period,uint16_t prescaler);
static void Delay ( __IO uint32_t nCount );
static void mount_filesystem(void);
//int mcu_connect_server(void);
void LCD_Display(long double result[3]);
void LCD_Display_InitInfo(char info[100]);
void LCD_Display_XmitInfo(char info[100]);
void LCD_Display_Init(void);
void mcu_xmit_ads1256data(void);
void mcu_open_savefile(void);
void mcu_save_ads1256data(void);

void Data_Collection_And_Xmit(void);
void Init_Rtc_Timer(void);
int If_TimerOut(int Current_RTC_Minutes, int Timing);
void mcu_save_ads1256data_left(int len);

int mcu_connect_udp_server(void);
/* Private functions ---------------------------------------------------------*/

/******************************/

FATFS fs;													/* FatFs文件系统对象 */
FIL adsfnew[3];													/* 文件对象 */
FRESULT res_sd;                /* 文件操作结果 */
UINT fnum;   
FRESULT ads1256_sd = FR_NO_FILE;

extern char result[3];
extern uchar a;
extern unsigned long AD_DATA;
extern unsigned int Ad_Result[3];

#define KEEP_CONNECT_TIMER     30000

#define AD_DATA_NUM 3072
//long double ADSVolutage[AD_DATA_NUM];
//long double ReadADSVolutage[AD_DATA_NUM];
unsigned int Ad_Data_Save[AD_DATA_NUM];
#define GET_AD_DATA_FROM_SDCARD_NUM 50
long double ReadADSVolutage[GET_AD_DATA_FROM_SDCARD_NUM] = {0};
unsigned int Ad_Data_Read[GET_AD_DATA_FROM_SDCARD_NUM] = {0};

static int LCD_LINE = 0;

//extern struct tcp_pcb *echoclient_pcb;
//extern struct echoclient * echoclient_es;
int Start_ADC_Flag = 0;

extern struct udp_pcb *upcb;
extern int Connect_Server_Flag;
/******************************/

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	//uint8_t flag=0;
	//int i = 0;
	/* 初始化LED */
	char serverinfo[100];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);	
	LED_GPIO_Config();
	
	/* 初始化按键 */
	Key_GPIO_Config();
	
	/* 初始化调试串口，一般为串口1 */
	Debug_USART_Config();
	//_485_Config();
	
	ILI9806G_Init ();         //LCD 初始化
	ILI9806G_GramScan ( 6 );
	printf("ILI9806G_GramScan OK\n");
	LCD_Display_Init();
	LCD_Display_InitInfo("Init Adc1256 Gpio Start...");
	Init_ADS_GPIO_NO_SPI();
	//Init_ADS_GPIO();
	LCD_Display_InitInfo("Init Adc1256 Gpio Success...");
	printf("\r\n **************init ads gpio ok********** \r\n");
	//GPIO_SetBits(GPIOB,GPIO_Pin_14);//reset 
	Delay(0x1ffFF);
	//SetADS1256_RESET1;
	//ADS1256_Init();
	LCD_Display_InitInfo("Init Adc1256 Register Start...");
	ADS1256_Init_No_SPI();
	//SetADS1256_RESET1;
	//ADS1256_Init();
	LCD_Display_InitInfo("Init Adc1256 Register Success...");
	printf("\r\n **************init ads ok********** \r\n");
	
	/* 初始化系统滴答定时器 */	
	SysTick_Init();
	
	TIM3_Config(999,899);//10ms定时器
	TIM4_Config(8999,9999);
	/* Configure ethernet (GPIOs, clocks, MAC, DMA) */
	LCD_Display_InitInfo("Init Eth Bsp Configure Start...");
  ETH_BSP_Config();	
	LCD_Display_InitInfo("Init Eth Bsp Configure Success...");
  printf("PHY初始化结束\n");
	
  /* Initilaize the LwIP stack */
	LCD_Display_InitInfo("Init LwIP Start...");
  LwIP_Init();	
	LCD_Display_InitInfo("Init LwIP Success...");
	Delay(0xfffFF);
  
	//Init_Rtc_Timer();
	
	mount_filesystem();
	LCD_Display_InitInfo("Mount Filesystem Success...");
	Delay(0xffFF);
	//mcu_open_savefile();
	LCD_Display_InitInfo("Mcu Open Save File Success...");
  
  /* IP地址和端口可在netconf.h文件修改，或者使用DHCP服务自动获取IP
	(需要路由器支持)*/
	sprintf(serverinfo, "Local IP:%d.%d.%d.%d", IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
	LCD_Display_InitInfo(serverinfo);
  printf("本地IP和端口: %d.%d.%d.%d\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
	sprintf(serverinfo, "Server IP:%d.%d.%d.%d:%d", DEST_IP_ADDR0, DEST_IP_ADDR1,DEST_IP_ADDR2, DEST_IP_ADDR3,DEST_PORT);
	LCD_Display_InitInfo(serverinfo);
  printf("远端IP和端口: %d.%d.%d.%d:%d\n",DEST_IP_ADDR0, DEST_IP_ADDR1,DEST_IP_ADDR2, DEST_IP_ADDR3,DEST_PORT);
  //mcu_connect_server();
	mcu_connect_udp_server();
	//printf("echoclient_pcb->state = %d\n",echoclient_pcb->state);
	printf("\r\n connect upcb->flag = %d\n", upcb->flags);
	
	/* wait server cmd */
  while(1)
	{
		//printf("\r\nwhile 1 connect server success echoclient_pcb->state = %d\n",echoclient_pcb->state);
		/* if not connect ,connect server again */
		/*
		if(echoclient_pcb != NULL && echoclient_pcb->state == FIN_WAIT_1){
		   if( mcu_connect_server() == ERR_OK)
			    printf("\r\nmcu connect server success echoclient_pcb->state = %d\n",echoclient_pcb->state);
			 else
				  printf("\r\nmcu connect server failed echoclient_pcb->state = %d\n",echoclient_pcb->state);
		}
		*/
		
		if(LocalTime - KEEPCONNECTTime >= KEEP_CONNECT_TIMER){
		    KEEPCONNECTTime = LocalTime;
			  if(Connect_Server_Flag == 0){  //every 30s connect server
			      if( mcu_connect_udp_server() == ERR_OK)
			          printf("\r\ntimer mcu connect server success upcb->flags = %d, upcb->local_port = %d Connect_Server_Flag = %d\n",upcb->flags,upcb->local_port,Connect_Server_Flag);
			      else
				        printf("\r\ntimer mcu connect server failed upcb->flags = %d, upcb->local_port = %d Connect_Server_Flag = %d\n",upcb->flags,upcb->local_port,Connect_Server_Flag);
		   }
		}
		
		//printf("\r\ncu connect server echoclient_pcb->state = %d\n",echoclient_pcb->state);
    
    if (ETH_CheckFrameReceived())
    { 
      LwIP_Pkt_Handle();
    }
    /* handle periodic timers for LwIP */
    LwIP_Periodic_Handle(LocalTime);
		
	}
	
}

void Data_Collection_And_Xmit()
{
  //long ulResult;
	//long double ldVolutage[3];
	//long double temADSVolutage;
	unsigned char channel;
	static int AD_Count = 0;
	//static int Current_RTC_Minutes = 0;
  //RTC_TimeTypeDef RTC_TimeStructure;
	//RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	//Current_RTC_Minutes = RTC_TimeStructure.RTC_Minutes;
	LED_BLUE;
	mcu_open_savefile();
	do
	{
		channel = POSITIVE_AIN0+NEGTIVE_AINCOM;
		Get_Ads1256_Data_No_SPI(channel);
    //Ad_Data_Save[AD_Count++] =  Get_chx_dat(channel);
		//ulResult = Ad_Result[0];
		Ad_Data_Save[AD_Count++] =  Ad_Result[0];
		Ad_Data_Save[AD_Count++] =  Ad_Result[1];
		//ulResult = Ad_Result[2];
		Ad_Data_Save[AD_Count++] =  Ad_Result[2];
		//tcp_xmit_ads1256data(echoclient_pcb,Ad_Result[2]);
		//printf("\r\n The current AD value = %d\r\n",Ad_Result[1]);
		if(AD_Count == AD_DATA_NUM){
		    mcu_save_ads1256data();
			  AD_Count = 0;
	  }
	
		//printf("\r\n The current AD value = %d\r\n",AD_Count);
   if (ETH_CheckFrameReceived())
    { 
      LwIP_Pkt_Handle();
    }
    LwIP_Periodic_Handle(LocalTime);
		
		//LCD_Display(ldVolutage);
	}while(Start_ADC_Flag);
	
	mcu_save_ads1256data_left(AD_Count);
	
	f_close(&adsfnew[0]); //save ok ,close the file	
	mcu_xmit_ads1256data();
	
}


int If_TimerOut(int Current_RTC_Minutes, int Timing)
{
  RTC_TimeTypeDef RTC_TimeStructure;
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	if(RTC_TimeStructure.RTC_Minutes - Current_RTC_Minutes >= Timing){
		
		return 0;
	}
	else
		return 1;
}
/**
  * @brief  通用定时器3中断初始化
  * @param  period : 自动重装值。
  * @param  prescaler : 时钟预分频数
  * @retval 无
  * @note   定时器溢出时间计算方法:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=定时器工作频率,为SystemCoreClock/2=90,单位:Mhz
  */
static void TIM3_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=period;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void TIM4_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=period;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	//TIM_Cmd(TIM4,ENABLE); //使能定时器4
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  定时器3中断服务函数
  * @param  无
  * @retval 无
  */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		LocalTime+=10;//10ms增量
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
static int TIM4_SEC = 0;
int Coll_Duration = 60;
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{
      TIM4_SEC++;
		  if(TIM4_SEC == Coll_Duration){
			  Start_ADC_Flag = 0;
				TIM4_SEC = 0;
				TIM_Cmd(TIM4,DISABLE);
			}
			
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
	
}


/**
  * @brief  简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

void Init_Rtc_Timer(void)
{
    RTC_CLK_Config();
	if (RTC_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)
  {
    /* 设置时间和日期 */
		RTC_TimeAndDate_Set();
  }
	  else
  {
    /* 检查是否电源复位 */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("\r\n 发生电源复位....\r\n");
    }
    /* 检查是否外部复位 */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("\r\n 发生外部复位....\r\n");
    }

    printf("\r\n 不需要重新配置RTC....\r\n");
    
    /* 使能 PWR 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
    PWR_BackupAccessCmd(ENABLE);
    /* 等待 RTC APB 寄存器同步 */
    RTC_WaitForSynchro();   
  } 
}

static void mount_filesystem(void)
{
   /*  挂载文件系统，保存AD的数据 */
	printf("\r\n**********mount filesystem***********\r\n");
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			printf("》SD卡已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_sd = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
			LED_RED; // if failed red power on
			printf("《《格式化失败。》》\r\n");
		}
	}
  else if(res_sd!=FR_OK)
  {
    printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
    printf("！！可能原因：SD卡初始化不成功。\r\n");
  }
  else
  {
		LED_GREEN;
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }
	
}

void mcu_open_savefile()
{
  ads1256_sd = f_open(&adsfnew[0], "0:ADS1256chip1.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if(ads1256_sd == FR_OK)
	{
		  LED_GREEN;
	    printf("》打开/创建ADS1256chip1.txt文件成功，向文件写入数据。\r\n");
	}
	else
	{	
		LED_RED;
		printf("！！打开/创建文件失败 ADS1256chip1.txt。\r\n");
	}
	
}


void mcu_save_ads1256data()
{
  //res_sd=f_write(&adsfnew[0],ADSVolutage,sizeof(ADSVolutage),&fnum);
	res_sd=f_write(&adsfnew[0],Ad_Data_Save,sizeof(Ad_Data_Save),&fnum);
	if(res_sd==FR_OK)
  {
	    LED_BLUE;
      //printf("》文件写入成功，写入字节数据：%d\n",fnum);
  }
  else
  {
		  LED_RED; 
      //printf("！！文件写入失败：(%d)\n",res_sd);
  }    
		/* 不再读写，关闭文件 */
	//LED_GREEN;
  //f_close(&adsfnew[0]);
}

void mcu_save_ads1256data_left(int len)
{
  //res_sd=f_write(&adsfnew[0],ADSVolutage,sizeof(ADSVolutage),&fnum);
	res_sd=f_write(&adsfnew[0],Ad_Data_Save,len,&fnum);
	if(res_sd==FR_OK)
  {
	    LED_GREEN;
      //printf("》文件写入成功，写入字节数据：%d\n",fnum);
  }
  else
  {
		  LED_RED; 
      //printf("！！文件写入失败：(%d)\n",res_sd);
  }    
		/* 不再读写，关闭文件 */
	//LED_GREEN;
  //f_close(&adsfnew[0]);
}
/*
int mcu_connect_server()
{
	  if (EthLinkStatus == 0){
			  printf("mcu_connect_server\n");
        return tcp_echoclient_connect();
		}
		return -1;
}
*/
int mcu_connect_udp_server()
{
	  if (EthLinkStatus == 0){
			  printf("mcu connect udp server\n");
        udp_echoclient_connect();
			  return 0;
		}
		return -1;
}

static int xmit_succ_count = 0, xmit_fail_count = 0;
void mcu_xmit_ads1256data()
{
  int i= 0;
	int count = 0;
  char xmit_info[100] = "Xmit Ad Data Succ:";
	FRESULT res = FR_OK;   
	UINT fnum =1;
	LED_RED;
	//if(echoclient_pcb != NULL && echoclient_pcb->state == FIN_WAIT_1)
  //    mcu_connect_server();
	
	res_sd = f_open(&adsfnew[0], "0:ADS1256chip1.txt", FA_OPEN_EXISTING | FA_READ); 
  
  if(res_sd == FR_OK)
	{
		LED_GREEN;
		printf("》打开文件成功。\r\n");
		//tcp_xmit_ads1256flag(echoclient_pcb,"Start,");
		udp_xmit_flag(upcb, "Start");
		for(;;){
		    res = f_read(&adsfnew[0], Ad_Data_Read, sizeof(Ad_Data_Read), &fnum);
		    if (res || fnum == 0) 
			    break;
				
        //printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
				count = count + fnum/4;
			  for(i = 0; i < fnum/4; i++)
	      {
					ReadADSVolutage[i] = (long double)Ad_Data_Read[i]*2.5/16777216*4;
					//printf("》读到字节数据： i=%d  %Lf\r\n",i,ReadADSVolutage[i]);
	        //if(tcp_xmit_ads1256data(echoclient_pcb,ReadADSVolutage[i]) == ERR_OK){
					//if(udp_xmit_ads1256data(upcb,ReadADSVolutage[i]) == ERR_OK){
				   // xmit_succ_count++;
			    //}
			    //else{
				   // xmit_fail_count++;
			    //}
			
	      }
				
				if(udp_xmit_ads1256data(upcb,ReadADSVolutage,fnum/4) == ERR_OK)
				    xmit_succ_count+=fnum/4;
				else
					  xmit_fail_count+=fnum/4;
				
				if (ETH_CheckFrameReceived())
          LwIP_Pkt_Handle();
    
        LwIP_Periodic_Handle(LocalTime);
				//Delay(0xfffff);
	    }		
	}
	else
	{
		LED_RED;
		printf("！！打开文件失败。\r\n");
	}
	f_close(&adsfnew[0]);
	ads1256_sd = FR_NO_FILE;
	udp_xmit_flag(upcb, "End");
	//tcp_xmit_ads1256flag(echoclient_pcb,"End");
	sprintf(xmit_info, "Data Count:%d", count);
	LCD_Display_InitInfo(xmit_info);
	sprintf(xmit_info, "Data Success:%d", xmit_succ_count);
	LCD_Display_InitInfo(xmit_info);
	sprintf(xmit_info, "Data Failed:%d", xmit_fail_count);
	LCD_Display_InitInfo(xmit_info);
	//tcp_echoclient_disconnect();
	//udp_echoclient_disconnect();
	Connect_Server_Flag = 0;
	KEEPCONNECTTime = LocalTime;
	xmit_succ_count = xmit_fail_count = 0;
	memset(Ad_Data_Read,0,100);
	memset(ReadADSVolutage,0,100);
	printf("\r\ndisconnect upcb->flag = %d\n",upcb->flags);
	Delay(0xffFF);
}

/*用于测试各种液晶的函数*/

void LCD_Display(long double result[3])
{
	

	static uint8_t testCNT = 0;	
	char dispBuff[100] = "ADC1256 data:";
	
	testCNT++;	
	
	//LCD_SetFont(&Font16x32);
	//LCD_SetColors(RED,BLACK);

  //ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
  /*
  ILI9806G_DispStringLine_EN(LINE(0),"BH 4.5 inch LCD");
  ILI9806G_DispStringLine_EN(LINE(2),"resolution:480x854px");
  ILI9806G_DispStringLine_EN(LINE(3),"LCD driver:ILI9806G");
  ILI9806G_DispStringLine_EN(LINE(4),"Touch driver:GT5688");
	*/
	
	ILI9806G_DispStringLine_EN(LINE(LCD_LINE),dispBuff);
	sprintf(dispBuff, "chip1:%Lf", result[0]);
	ILI9806G_DispStringLine_EN(LINE(LCD_LINE+1),dispBuff);
	sprintf(dispBuff, "chip2:%Lf", result[1]);
	ILI9806G_DispStringLine_EN(LINE(LCD_LINE+2),dispBuff);
	sprintf(dispBuff, "chip3:%Lf", result[2]);
	ILI9806G_DispStringLine_EN(LINE(LCD_LINE+3),dispBuff);
  
}

void LCD_Display_Init(void)
{
  LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);

  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	

  ILI9806G_DispStringLine_EN(LINE(LCD_LINE++),"9_ch_24bit system start init....");
}

void LCD_Display_InitInfo(char info[100])
{	
	
  ILI9806G_DispStringLine_EN(LINE(LCD_LINE++),info);
}

void LCD_Display_XmitInfo(char info[100])
{	
  ILI9806G_DispStringLine_EN(LINE((LCD_LINE)),info);
}

