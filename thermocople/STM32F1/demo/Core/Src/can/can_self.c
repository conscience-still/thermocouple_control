#include "can_self.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "temp_ctrl.h"

void hex_str(unsigned char *inchar, unsigned int len, unsigned char *outtxt)
{
  unsigned char hbit,lbit;
	unsigned int i;
  for(i=0;i<len;i++)
	{
		hbit = (*(inchar+i)&0xf0)>>4;
		lbit = *(inchar+i)&0x0f;
		if (hbit>9)
			outtxt[2*i]='A'+hbit-10;
		else
			outtxt[2*i]='0'+hbit;
		if (lbit>9) 
			outtxt[2*i+1]='A'+lbit-10;
		else    
			outtxt[2*i+1]='0'+lbit;
  }
	outtxt[2*i] = 0;
}

//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 		   
void MY_NVIC_PriorityGroupConfig(uint8_t NVIC_Group)	 
{ 
	uint32_t temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}
//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����	   
void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_SubPriority,uint8_t NVIC_Channel,uint8_t NVIC_Group)	 
{ 
	uint32_t temp;	
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//ȡ����λ  
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//ʹ���ж�λ(Ҫ����Ļ�,�෴������OK) 
	NVIC->IP[NVIC_Channel]|=temp<<4;		//������Ӧ���ȼ����������ȼ�   	    	  				   
} 
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:1~3;
//tbs2:ʱ���2��ʱ�䵥Ԫ.��Χ:1~8;
//tbs1:ʱ���1��ʱ�䵥Ԫ.��Χ:1~16;
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//ע�����ϲ����κ�һ����������Ϊ0,�������.
//������=Fpclk1/((tbs1+tbs2+1)*brp);
//mode:0,��ͨģʽ;1,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Normal_Init(1,8,9,4,1);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;
uint8_t CAN_Mode_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode)
{
	uint16_t i=0;
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//�ȼ�ȥ1.����������
	tbs2-=1;
	tbs1-=1;
	brp-=1;

	RCC->APB2ENR|=1<<2;    	//ʹ��PORTAʱ��	 
	GPIOA->CRH&=0XFFF00FFF; 
	GPIOA->CRH|=0X000B8000;	//PA11 RX,PA12 TX�������   	 
    GPIOA->ODR|=3<<11;
					    
	RCC->APB1ENR|=1<<25;	//ʹ��CANʱ�� CANʹ�õ���APB1��ʱ��(max:36M)
	CAN1->MCR=0x0000;		//�˳�˯��ģʽ(ͬʱ��������λΪ0)
	CAN1->MCR|=1<<0;		//����CAN�����ʼ��ģʽ
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;	//�����ʼ��ģʽʧ��
	}
	CAN1->MCR|=0<<7;		//��ʱ�䴥��ͨ��ģʽ
	CAN1->MCR|=0<<6;		//����Զ����߹���
	CAN1->MCR|=0<<5;		//˯��ģʽͨ���������(���CAN1->MCR��SLEEPλ)
	CAN1->MCR|=1<<4;		//��ֹ�����Զ�����
	CAN1->MCR|=0<<3;		//���Ĳ�����,�µĸ��Ǿɵ�
	CAN1->MCR|=0<<2;		//���ȼ��ɱ��ı�ʶ������
	CAN1->BTR=0x00000000;	//���ԭ��������.
	CAN1->BTR|=mode<<30;	//ģʽ���� 0,��ͨģʽ;1,�ػ�ģʽ;
	CAN1->BTR|=tsjw<<24; 	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ
	CAN1->BTR|=tbs2<<20; 	//Tbs2=tbs2+1��ʱ�䵥λ
	CAN1->BTR|=tbs1<<16;	//Tbs1=tbs1+1��ʱ�䵥λ
	CAN1->BTR|=brp<<0;  	//��Ƶϵ��(Fdiv)Ϊbrp+1
							//������:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN1->MCR&=~(1<<0);		//����CAN�˳���ʼ��ģʽ
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//�˳���ʼ��ģʽʧ��
	}
	//��������ʼ��
	CAN1->FMR|=1<<0;		//�������鹤���ڳ�ʼ��ģʽ
	CAN1->FA1R&=~(1<<0);	//������0������
	CAN1->FS1R|=1<<0; 		//������λ��Ϊ32λ.
	CAN1->FM1R|=0<<0;		//������0�����ڱ�ʶ������λģʽ
	CAN1->FFA1R|=0<<0;		//������0������FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32λID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32λMASK
	CAN1->FA1R|=1<<0;		//���������0
	CAN1->FMR&=0<<0;		//���������������ģʽ

#if CAN_RX0_INT_ENABLE
 	//ʹ���жϽ���
	CAN1->IER|=1<<1;		//FIFO0��Ϣ�Һ��ж�����.	    
	MY_NVIC_Init(1,0,USB_LP_CAN1_RX0_IRQn,2);//��2
#endif
	return 0;
}   
//id:��׼ID(11λ)/��չID(11λ+18λ)	    
//ide:0,��׼֡;1,��չ֡
//rtr:0,����֡;1,Զ��֡
//len:Ҫ���͵����ݳ���(�̶�Ϊ8���ֽ�,��ʱ�䴥��ģʽ��,��Ч����Ϊ6���ֽ�)
//*dat:����ָ��.
//����ֵ:0~3,������.0XFF,����Ч����.
uint8_t CAN_Tx_Msg(uint32_t id,uint8_t ide,uint8_t rtr,uint8_t len,uint8_t *dat)
{	   
	uint8_t mbox;	  
	if(CAN1->TSR&(1<<26))mbox=0;		//����0Ϊ��
	else if(CAN1->TSR&(1<<27))mbox=1;	//����1Ϊ��
	else if(CAN1->TSR&(1<<28))mbox=2;	//����2Ϊ��
	else return 0XFF;					//�޿�����,�޷����� 
	CAN1->sTxMailBox[mbox].TIR=0;		//���֮ǰ������
	if(ide==0)	//��׼֡
	{
		id&=0x7ff;//ȡ��11λstdid
		id<<=21;		  
	}else		//��չ֡
	{
		id&=0X1FFFFFFF;//ȡ��32λextid
		id<<=3;									   
	}
	CAN1->sTxMailBox[mbox].TIR|=id;		 
	CAN1->sTxMailBox[mbox].TIR|=ide<<2;	  
	CAN1->sTxMailBox[mbox].TIR|=rtr<<1;
	len&=0X0F;//�õ�����λ
	CAN1->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[mbox].TDTR|=len;	//����DLC.
	//���������ݴ�������.
	CAN1->sTxMailBox[mbox].TDHR=(((uint32_t)dat[7]<<24)|
								((uint32_t)dat[6]<<16)|
 								((uint32_t)dat[5]<<8)|
								((uint32_t)dat[4]));
	CAN1->sTxMailBox[mbox].TDLR=(((uint32_t)dat[3]<<24)|
								((uint32_t)dat[2]<<16)|
 								((uint32_t)dat[1]<<8)|
								((uint32_t)dat[0]));
	CAN1->sTxMailBox[mbox].TIR|=1<<0; 	//��������������
	return mbox;
}
//��÷���״̬.
//mbox:������;
//����ֵ:����״̬. 0,����;0X05,����ʧ��;0X07,���ͳɹ�.
uint8_t CAN_Tx_Staus(uint8_t mbox)
{	
	uint8_t sta=0;					    
	switch (mbox)
	{
		case 0: 
			sta |= CAN1->TSR&(1<<0);		//RQCP0
			sta |= CAN1->TSR&(1<<1);		//TXOK0
			sta |=((CAN1->TSR&(1<<26))>>24);//TME0
			break;
		case 1: 
			sta |= CAN1->TSR&(1<<8)>>8;		//RQCP1
			sta |= CAN1->TSR&(1<<9)>>8;		//TXOK1
			sta |=((CAN1->TSR&(1<<27))>>25);//TME1	   
			break;
		case 2: 
			sta |= CAN1->TSR&(1<<16)>>16;	//RQCP2
			sta |= CAN1->TSR&(1<<17)>>16;	//TXOK2
			sta |=((CAN1->TSR&(1<<28))>>26);//TME2
			break;
		default:
			sta=0X05;//����Ų���,�϶�ʧ��.
		break;
	}
	return sta;
} 
//�õ���FIFO0/FIFO1�н��յ��ı��ĸ���.
//fifox:0/1.FIFO���;
//����ֵ:FIFO0/FIFO1�еı��ĸ���.
uint8_t CAN_Msg_Pend(uint8_t fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}
//��������
//fifox:�����
//id:��׼ID(11λ)/��չID(11λ+18λ)	    
//ide:0,��׼֡;1,��չ֡
//rtr:0,����֡;1,Զ��֡
//len:���յ������ݳ���(�̶�Ϊ8���ֽ�,��ʱ�䴥��ģʽ��,��Ч����Ϊ6���ֽ�)
//dat:���ݻ�����
void CAN_Rx_Msg(uint8_t fifox,uint32_t *id,uint8_t *ide,uint8_t *rtr,uint8_t *len,uint8_t *dat)
{	   
	*ide=CAN1->sFIFOMailBox[fifox].RIR&0x04;	//�õ���ʶ��ѡ��λ��ֵ  
 	if(*ide==0)//��׼��ʶ��
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>21;
	}else	   //��չ��ʶ��
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>3;
	}
	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;	//�õ�Զ�̷�������ֵ.
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;	//�õ�DLC
 	//*fmi=(CAN1->sFIFOMailBox[FIFONumber].RDTR>>8)&0xFF;//�õ�FMI
	//��������
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN1->RF0R|=0X20;//�ͷ�FIFO0����
	else if(fifox==1)CAN1->RF1R|=0X20;//�ͷ�FIFO1����	 
}

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
uint8_t CAN_Send_Msg(uint8_t* msg,uint8_t len)
{	
	uint8_t mbox;
	uint16_t i=0;	  	 						       
    mbox=CAN_Tx_Msg(0X12,0,0,len,msg);
	while((CAN_Tx_Staus(mbox)!=0X07)&&(i<0XFFF))i++;//�ȴ����ͽ���
	if(i>=0XFFF)return 1;							//����ʧ��?
	return 0;										//���ͳɹ�;
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
uint8_t CAN_Receive_Msg(uint8_t *buf)
{		   		   
	uint32_t id;
	uint8_t ide,rtr,len; 
	if(CAN_Msg_Pend(0)==0)return 0;			//û�н��յ�����,ֱ���˳� 	 
  	CAN_Rx_Msg(0,&id,&ide,&rtr,&len,buf); 	//��ȡ����
    if(id!=0x12||ide!=0||rtr!=0)len=0;		//���մ���	   
	return len;	
}

/***************************************************************************************/

CAN_RecvTypeDef CAN_RecvInf; 
// CAN_FIFO0 0 
#if CAN_RX0_INT_ENABLE      	//ʹ��RX0�ж�
//�жϷ�����			  
void CAN1_RX0_IRQHandler(void)
{
	  CAN_RecvInf.CRecvFlag0=1;
	  CAN_Rx_Msg(0,&CAN_RecvInf.RecvMessage0.StdId,&CAN_RecvInf.RecvMessage0.IDE,
		&CAN_RecvInf.RecvMessage0.RTR,&CAN_RecvInf.RecvMessage0.DLC,CAN_RecvInf.RecvMessage0.Data);
//	  NVIC_DisableIRQ(CAN1_RX0_IRQn);
	  #if 1 
		printf("Rx ID��0x%x \r\n",CAN_RecvInf.RecvMessage0.StdId);
//		printf("ide:%d\r\n",CAN_RecvInf.RecvMessage0.IDE);
//		printf("rtr:%d\r\n",CAN_RecvInf.RecvMessage0.RTR);
//		printf("len:%d\r\n",CAN_RecvInf.RecvMessage0.DLC);
//		for(uint8_t i=0;i<CAN_RecvInf.RecvMessage0.DLC;i++)
//			printf("rxbuf[0]:%d\r\n",CAN_RecvInf.RecvMessage0.Data[i]);

	#endif
}
#endif

void SendBuf_CAN(uint32_t id,uint8_t* p,uint8_t len)
{	
//ide:0,��׼֡;1,��չ֡
//rtr:0,����֡;1,Զ��֡
	uint8_t mbox;
    mbox=CAN_Tx_Msg(id,0,0,len,p);
	while(1)
	{
		if(CAN_Tx_Staus(mbox)!=0X07)//�ж�״̬
          break;
		else
		{
			printf("TxStatus_Failed!\r\n ");
			return;
		}
	}	
	#if 1 //CAN_DPRINT
	uint8_t *str = malloc(8);
	hex_str(p,8,str);
	printf("CANSend--->:%s\r\n",str);
	free(str);
	#endif
}
/*�����������*/
void RecvCTRL_210(uint8_t* msg)
{
	CANRecv210* p1;
	p1 = (CANRecv210*)msg;
	can_ctrl.ctrl_num = p1->ctrl_sta;
	
	Clear_Ctrl_Sta(can_ctrl.ctrl_num);		

}

CANAnalyzTypeDef CANAnalyzTab[]={
	 { 0x210,*RecvCTRL_210      },
};
//����: CAN���յ���Ϣ�󣬽����ⲿɨ������
//���룺CanRxMsg
//�������Ӧ��ȫ�ֱ���
//ע��: 
//��  �ߣ�lyn
uint8_t CANScan(void)
{

	if(CAN_RecvInf.CRecvFlag0)
	{
		for(uint8_t i=0;i<sizeof(CANAnalyzTab)/sizeof(CANAnalyzTypeDef);i++)
		{						 
			 if(CAN_RecvInf.RecvMessage0.StdId == CANAnalyzTab[i].CAN_ID)//����EMS_EngineID ת�� ��ѹ����Ϣ
			 {
//				 printf(".Rx CAN_ID:0x%x \r\n",CAN_RecvInf.RecvMessage0.StdId);
				 CANAnalyzTab[i].f(CAN_RecvInf.RecvMessage0.Data);
				 CAN_RecvInf.CRecvFlag0=0;
//				NVIC_EnableIRQ(CAN1_RX0_IRQn);
			 }
		}
	}
  return 1;
}

void CAN_SendMsg_226(void)
{
	uint8_t *p=malloc(sizeof(uint8_t)*8);
	CANSend226 *p1;
	p1 = (CANSend226*)p;	
	p1->ctrl_sta_one = CTRL_ONE_GPIO_Port->IDR & CTRL_ONE_Pin?true:false;
	p1->ctrl_tim_one = 	Tim_Cnt[0].count ==0? 0:Tim_Cnt[0].count/60+1 ; //��λ ����
//	printf("tim min:%d.%d\n",Tim_Cnt[0].count,Tim_Cnt[0].count/60+1);
	p1->ctrl_sta_two = CTRL_TWO_GPIO_Port->IDR & CTRL_TWO_Pin?true:false;
	p1->ctrl_tim_two = Tim_Cnt[1].count ==0? 0:Tim_Cnt[1].count/60+1 ; //��λ ����;
	p1->ctrl_sta_three = CTRL_THREE_GPIO_Port->IDR & CTRL_THREE_Pin?true:false;
	p1->ctrl_tim_three = Tim_Cnt[3].count ==0? 0:Tim_Cnt[2].count/60+1 ; //��λ ����
	p1->ctrl_sta_four = CTRL_FOUR_GPIO_Port->IDR & CTRL_FOUR_Pin ?true:false;
	p1->ctrl_tim_four = Tim_Cnt[4].count ==0? 0:Tim_Cnt[4].count/60+1 ; //��λ ����
	p1->ctrl_sta_five = CTRL_FIVE_GPIO_Port->IDR & CTRL_FIVE_Pin?true:false;
	p1->ctrl_tim_five = Tim_Cnt[5].count ==0? 0:Tim_Cnt[5].count/60+1 ; //��λ ����
	p1->ctrl_sta_six = CTRL_SIX_GPIO_Port->IDR & CTRL_SIX_Pin?true:false;
	p1->ctrl_tim_six = Tim_Cnt[6].count ==0? 0:Tim_Cnt[6].count/60+1 ; //��λ ����
	p1->ctrl_sta_seven = CTRL_SEVEN_GPIO_Port->IDR & CTRL_SEVEN_Pin?true:false;
	p1->ctrl_tim_seven = Tim_Cnt[6].count ==0? 0:Tim_Cnt[6].count/60+1 ; //��λ ����
	p1->ctrl_sta_eight = CTRL_EIGHT_GPIO_Port->IDR & CTRL_EIGHT_Pin?true:false;
	p1->ctrl_tim_eight = Tim_Cnt[7].count ==0? 0:Tim_Cnt[7].count/60+1 ; //��λ ����
	SendBuf_CAN(0x226,p,8);
	free(p);	
	
}
extern CANSend33B cansend33b_t_msg;
void CAN_SendMsg_33B(void)
{
	uint8_t *p=malloc(sizeof(uint8_t)*8);
	CANSend33B *p1;
	p1 = (CANSend33B*)p;	
	p1->Temperature_one = 	cansend33b_t_msg.Temperature_one;
	p1->Temperature_Two = cansend33b_t_msg.Temperature_Two;
	p1->Temperature_Three = cansend33b_t_msg.Temperature_Three;
	p1->Temperature_Four = cansend33b_t_msg.Temperature_Four;
	p1->Temperature_Five = cansend33b_t_msg.Temperature_Five;
	p1->Temperature_Six = cansend33b_t_msg.Temperature_Six;
	p1->Temperature_Seven = cansend33b_t_msg.Temperature_Seven;
	p1->Temperature_Eight = cansend33b_t_msg.Temperature_Eight;
	SendBuf_CAN(0x33B,p,8);
	free(p);	
}
CAN_SendTypeDef CAN_Send_Tab[] =
{
	{0x226 ,0,3 ,0,CAN_SendMsg_226},
	{0x33B ,0,3 ,0,CAN_SendMsg_33B},
};
void SendCTRL_210(void)
{
	uint8_t *p=malloc(sizeof(uint8_t)*8);
	CANRecv210* p1;
	p1 = (CANRecv210*)p;	
	p1->ctrl_sta = 1;
	SendBuf_CAN(0x210,p,8);
	free(p);
}
//�������ܣ��������ݵ�CAN����
//��  �ߣ�lyn
CAN_Send_general_fun CAN_ProSend(void)
{
	for(uint8_t i=0;i<sizeof(CAN_Send_Tab)/sizeof(CAN_SendTypeDef);i++)
	{
		 if(CAN_Send_Tab[i].flag == 0)
		 {
			 SendCTRL_210();
			 CAN_Send_Tab[i].CAN_Send_Fun();
			 CAN_Send_Tab[i].flag =1;
			 #if 0
			printf(".Tx CAN_ID:0x%03x \r\n",CAN_Send_Tab[i].Type);
			return CAN_Send_Tab[i].CAN_Send_Fun;//����ֵ������Խ�����ѯ���� lyn
			 #endif
		 }
	 }
	return 0;
}

void CANSendTime(void)
{
	for(char i=0;i<sizeof(CAN_Send_Tab)/ sizeof(CAN_Send_general_fun);i++)
	{
		if(CAN_Send_Tab[i].flag == 1)
		{	
			(CAN_Send_Tab[i].count < CAN_Send_Tab[i].src)?(CAN_Send_Tab[i].count ++):(CAN_Send_Tab[i].flag = 0,CAN_Send_Tab[i].count=0);
		}
	}
}


