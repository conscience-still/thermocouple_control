#include "sys.h"
#include "delay.h"
#include "uart.h"	  
#include "can.h"
#include "HAL_can.h"
#include "adc.h"
#include "tim.h"
#include "led.h"
#include "temprate.h"
#include "HAL_conf.h"

CanTxMsg CAN_TX_Config ={TX_CANID,CAN_ID_STD,CAN_DATA_FRAME,8,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
CanPeliRxMsg CanPeliRxMsgStructure;
extern u32 SystemCoreClock;
unsigned int ucTim2Flag=0;

/********************************************************************************************************
**������Ϣ ��int main (void)                          
**�������� �������󣬽��յ����Ĳ���ӡ
**������� ��
**������� ��
********************************************************************************************************/

int main(void)
{
	u8 i = 0;	
	u16 ADCVAL;
	float fValue;	
	unsigned int uiCnt=0;

	delay_init();
	LED_Init();
//	uart_initwBaudRate(115200);	 	//���ڳ�ʼ��Ϊ115200
	CANM_Init();
	ADC1_SingleChannel(ADC_Channel_1);
	Tim2_UPCount_test(SystemCoreClock/10000-1,9999);

	CAN_Config(CAN_250K,ExtendedFrame_DoubleFilter,0x172,0x325,0x00,0x00);		//CAN�������μĴ�������λ����Ϊ��أ���ֻ�ɽ������մ���Ĵ����е�����CAN_ID
  while(1)
  {
//    Send_CANFrame(&CAN_TX_Config);								//����һ֡CAN����
//	  	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
//	  	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
//	  	GPIO_ResetBits(GPIOB,GPIO_Pin_2);
//		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_SetBits(GPIOB,GPIO_Pin_0);

//    delay_ms(300);
//	GPIO_SetBits(GPIOA,GPIO_Pin_9);
//	GPIO_SetBits(GPIOA,GPIO_Pin_2);
//	GPIO_SetBits(GPIOB,GPIO_Pin_2);
//	GPIO_SetBits(GPIOA,GPIO_Pin_15);
	  
//    delay_ms(300);

	  if(flag)																	
    {
      flag = 0;
      printf("CANID:0x%x  Data",CanPeliRxMsgStructure.ID);	//���ڴ�ӡ
      for(i=0;i<8;i++)															
      {
        printf("%x",CanPeliRxMsgStructure.Data[i]);		//��ӡCAN��������
      }
      printf("\r\n");																		//����
    } 
	
	ADCVAL=Get_Adc_Average(ADC_Channel_1,5);
	fValue = ((float)ADCVAL/4095)*3.3;
	printf("ADC1_CH_1=%fV\r\n",fValue);
	delay_ms(200);  
	

	if(ucTim2Flag >999)
	{
		ucTim2Flag=0;
		
		/*��ӡ��ǰuiCntֵ */
		printf("Time=0x%ds\r\n",
		uiCnt);
		uiCnt++;
	}		
//	for(uiCnt=0; ;uiCnt++)
//	{
//		/* �ȴ���ʱ����� */

//		Tim2_UPStatusOVCheck_test();
//		
//		/* ��ӡ��ǰ��ֵ */
//		printf("0x%d\r\n",uiCnt);
//	}	

  }
}




