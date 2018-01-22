/**
  ******************************************************************************
  * @file    udp_echoclient.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   UDP echo client
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "netconf.h"
//#include "LAN8742A.h"
#include "stm32f4x7_phy.h"
#include "lwip/pbuf.h"
//#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "udp_echoclient.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
extern void Data_Collection_And_Xmit(void);
uint8_t   data_udp[100];
__IO uint32_t message_count_udp = 0;

struct udp_pcb *upcb = NULL;

extern int Coll_Duration;
extern int Start_ADC_Flag;
void Analysis_Cmd_Data(char *CmdData)
{
	  char *Duration;
	  char *SPS;
	  char *Start_Time;
	  char *End_Time;
	  char *result;
	  printf("CmdData: %s\n",CmdData);
	  result = strtok(CmdData,":");
	  result = strtok(NULL,":");
	  if(strcmp(result,"Manual") == 0){
		    Duration = strtok(NULL,":");
			  Coll_Duration = atoi(Duration)*60;
			  printf("Duration: %s Coll_Duration: %d\n",Duration,Coll_Duration);
			  SPS = strtok(NULL,":");
			  //printf("SPS %s, atoi = %d\n",SPS,atoi(SPS));
			  Start_ADC_Flag = 1;
				TIM_Cmd(TIM4,ENABLE); 
				Data_Collection_And_Xmit();
		}
		else if(strcmp(result,"Auto") == 0){
		    Start_Time = strtok(NULL,":");
			  printf("Start Time %s\n",Start_Time);
			  End_Time = strtok(NULL,":");
			  printf("End Time %s\n",End_Time);
			  SPS = strtok(NULL,":");
			  printf("SPS %s\n",SPS);
		}
		else
			  printf("Invalid Cmd Data\n");
}

int udp_xmit_ads1256data(struct udp_pcb *upcb, long double *ReadADSVolutage,int len /*unsigned int ReadADSVolutage*/)
{
  //struct echoclient *es = NULL;
  char dispBuff[450] = {0};
	int pos = 0,i;
	struct pbuf *p;
	err_t ret_err;
	if (upcb != NULL)
	{
		  for(i = 0; i < len; i++){
		      pos += sprintf(dispBuff+pos, "%Lf,", ReadADSVolutage[i]);
			}
		  p = pbuf_alloc(PBUF_TRANSPORT, strlen(dispBuff) , PBUF_POOL);
		  if(p != NULL){
				  pbuf_take(p, dispBuff, strlen(dispBuff));
			    ret_err = udp_send(upcb,p);
				  pbuf_free(p);
				  return ret_err;
			}
			else
				return ERR_MEM;
	}
	return ERR_CONN;  
}

int udp_xmit_flag(struct udp_pcb *upcb, char dispBuff[20] /*unsigned int ReadADSVolutage*/)
{
  //struct echoclient *es = NULL;
	struct pbuf *p;
	if (upcb != NULL)
	{
		  p = pbuf_alloc(PBUF_TRANSPORT, strlen(dispBuff) , PBUF_POOL);
		  if(p != NULL){
				  pbuf_take(p, dispBuff, strlen(dispBuff));
			    udp_send(upcb,p);
				  pbuf_free(p);
				  return ERR_OK;
			}
			else
				return ERR_MEM;
	}
	return ERR_CONN;  
}


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Connect to UDP echo server
  * @param  None
  * @retval None
  */
 void udp_echoclient_connect(void)
{
  struct pbuf *p;
  struct ip_addr DestIPaddr;
  err_t err;
       
  /* Create a new UDP control block  */
	if (upcb == NULL)
      upcb = udp_new();
  
  if (upcb!=NULL)
  {
    /*assign destination IP address */
    IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );
  
    /* configure destination IP address and port */
    err= udp_connect(upcb, &DestIPaddr, DEST_PORT);
    
    if (err == ERR_OK)
    {
      /* Set a receive callback for the upcb */
      udp_recv(upcb, udp_receive_callback, NULL);
      
      sprintf((char*)data_udp, "client:MCU%d\n",message_count_udp);
  
      /* allocate pbuf from pool*/
      p = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)data_udp), PBUF_POOL);
      
      if (p != NULL)
      {
        /* copy data to pbuf */
        pbuf_take(p, (char*)data_udp, strlen((char*)data_udp));
          
        /* send udp data */
        udp_send(upcb, p); 
        
        /* free pbuf */
        pbuf_free(p);
				
				//return ERR_OK;
      }
      else
      {
        /* free the UDP connection, so we can accept new clients */
        udp_remove(upcb);
        #ifdef SERIAL_DEBUG
        printf("\n\r can not allocate pbuf ");
        #endif
				//return ERR_MEM;
      }
    }
    else
    {
      /* free the UDP connection, so we can accept new clients */
      udp_remove(upcb);
      #ifdef SERIAL_DEBUG
       printf("\n\r can not connect udp pcb");
      #endif
			//return ERR_CONN;
    }
  }
  else
  {
    #ifdef SERIAL_DEBUG
     printf("\n\r can not create udp pcb");
    #endif
		//return ERR_CONN;
  } 
}


//extern void Analysis_Cmd_Data(char *CmdData);
int Connect_Server_Flag = 0;
/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
	char recdata[50] = {0};
	char analysis_data[30] = {0};
	char *result = NULL ;
	/*increment message count */
  //message_count_udp++;
	
	if(p !=NULL)
	{		
		#ifdef SERIAL_DEBUG
		//recdata=(char *)malloc(p->len*sizeof(char));
		
		//if(recdata!=NULL)
		{
			//memset(recdata,0,p->len*sizeof(char));
			
			memcpy(recdata,p->payload,p->len);
			//strcpy(recdata,p->payload);
			printf("upd_rec:%s",recdata);
      if(strcmp(recdata,"CONNECT SERVER SUCCESS ACK")==0){
          Connect_Server_Flag = 1;				
				  printf("\r\nConnect_Server_Flag = %d\n",Connect_Server_Flag);
			}
			else{
				//analysis_data = (char *)malloc(p->len*sizeof(char));
				//memset(analysis_data,0,p->len*sizeof(char));
				strcpy(analysis_data,p->payload);
			  result = strtok(recdata,":");
			  if(result)
			    printf("\r\nudp_echoclient_recv result = %s\n",result);
			  if((strcmp(recdata,"CMD") == 0) && Connect_Server_Flag){
				  Analysis_Cmd_Data(analysis_data);
			  }
				//free(analysis_data);
		  }
		}
		//free(recdata);
		
    #endif
		//udp_send(upcb,p);
		/* Free receive pbuf */
		pbuf_free(p);
	}
  
//  /* free the UDP connection, so we can accept new clients */
//  udp_remove(upcb);   
}

/**
  * @brief  Disconnect to UDP echo server
  * @param  None
  * @retval None
  */
void udp_echoclient_disconnect(void)
{
	#ifdef SERIAL_DEBUG
	printf("Disconnect to UDP echo server");
	#endif
	 //upcb->flags &= ~UDP_FLAGS_CONNECTED;
	 /* free the UDP connection, so we can accept new clients */
  udp_remove(upcb);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
