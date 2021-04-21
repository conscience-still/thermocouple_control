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

//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
void MY_NVIC_PriorityGroupConfig(uint8_t NVIC_Group)	 
{ 
	uint32_t temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}
//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_SubPriority,uint8_t NVIC_Channel,uint8_t NVIC_Group)	 
{ 
	uint32_t temp;	
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//取低四位  
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//使能中断位(要清除的话,相反操作就OK) 
	NVIC->IP[NVIC_Channel]|=temp<<4;		//设置响应优先级和抢断优先级   	    	  				   
} 
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~3;
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//波特率=Fpclk1/((tbs1+tbs2+1)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(1,8,9,4,1);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;
uint8_t CAN_Mode_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode)
{
	uint16_t i=0;
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//先减去1.再用于设置
	tbs2-=1;
	tbs1-=1;
	brp-=1;

	RCC->APB2ENR|=1<<2;    	//使能PORTA时钟	 
	GPIOA->CRH&=0XFFF00FFF; 
	GPIOA->CRH|=0X000B8000;	//PA11 RX,PA12 TX推挽输出   	 
    GPIOA->ODR|=3<<11;
					    
	RCC->APB1ENR|=1<<25;	//使能CAN时钟 CAN使用的是APB1的时钟(max:36M)
	CAN1->MCR=0x0000;		//退出睡眠模式(同时设置所有位为0)
	CAN1->MCR|=1<<0;		//请求CAN进入初始化模式
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;	//进入初始化模式失败
	}
	CAN1->MCR|=0<<7;		//非时间触发通信模式
	CAN1->MCR|=0<<6;		//软件自动离线管理
	CAN1->MCR|=0<<5;		//睡眠模式通过软件唤醒(清除CAN1->MCR的SLEEP位)
	CAN1->MCR|=1<<4;		//禁止报文自动传送
	CAN1->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN1->MCR|=0<<2;		//优先级由报文标识符决定
	CAN1->BTR=0x00000000;	//清除原来的设置.
	CAN1->BTR|=mode<<30;	//模式设置 0,普通模式;1,回环模式;
	CAN1->BTR|=tsjw<<24; 	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN1->BTR|=tbs2<<20; 	//Tbs2=tbs2+1个时间单位
	CAN1->BTR|=tbs1<<16;	//Tbs1=tbs1+1个时间单位
	CAN1->BTR|=brp<<0;  	//分频系数(Fdiv)为brp+1
							//波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN1->MCR&=~(1<<0);		//请求CAN退出初始化模式
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//退出初始化模式失败
	}
	//过滤器初始化
	CAN1->FMR|=1<<0;		//过滤器组工作在初始化模式
	CAN1->FA1R&=~(1<<0);	//过滤器0不激活
	CAN1->FS1R|=1<<0; 		//过滤器位宽为32位.
	CAN1->FM1R|=0<<0;		//过滤器0工作在标识符屏蔽位模式
	CAN1->FFA1R|=0<<0;		//过滤器0关联到FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32位ID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32位MASK
	CAN1->FA1R|=1<<0;		//激活过滤器0
	CAN1->FMR&=0<<0;		//过滤器组进入正常模式

#if CAN_RX0_INT_ENABLE
 	//使用中断接收
	CAN1->IER|=1<<1;		//FIFO0消息挂号中断允许.	    
	MY_NVIC_Init(1,0,USB_LP_CAN1_RX0_IRQn,2);//组2
#endif
	return 0;
}   
//id:标准ID(11位)/扩展ID(11位+18位)	    
//ide:0,标准帧;1,扩展帧
//rtr:0,数据帧;1,远程帧
//len:要发送的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
//*dat:数据指针.
//返回值:0~3,邮箱编号.0XFF,无有效邮箱.
uint8_t CAN_Tx_Msg(uint32_t id,uint8_t ide,uint8_t rtr,uint8_t len,uint8_t *dat)
{	   
	uint8_t mbox;	  
	if(CAN1->TSR&(1<<26))mbox=0;		//邮箱0为空
	else if(CAN1->TSR&(1<<27))mbox=1;	//邮箱1为空
	else if(CAN1->TSR&(1<<28))mbox=2;	//邮箱2为空
	else return 0XFF;					//无空邮箱,无法发送 
	CAN1->sTxMailBox[mbox].TIR=0;		//清除之前的设置
	if(ide==0)	//标准帧
	{
		id&=0x7ff;//取低11位stdid
		id<<=21;		  
	}else		//扩展帧
	{
		id&=0X1FFFFFFF;//取低32位extid
		id<<=3;									   
	}
	CAN1->sTxMailBox[mbox].TIR|=id;		 
	CAN1->sTxMailBox[mbox].TIR|=ide<<2;	  
	CAN1->sTxMailBox[mbox].TIR|=rtr<<1;
	len&=0X0F;//得到低四位
	CAN1->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[mbox].TDTR|=len;	//设置DLC.
	//待发送数据存入邮箱.
	CAN1->sTxMailBox[mbox].TDHR=(((uint32_t)dat[7]<<24)|
								((uint32_t)dat[6]<<16)|
 								((uint32_t)dat[5]<<8)|
								((uint32_t)dat[4]));
	CAN1->sTxMailBox[mbox].TDLR=(((uint32_t)dat[3]<<24)|
								((uint32_t)dat[2]<<16)|
 								((uint32_t)dat[1]<<8)|
								((uint32_t)dat[0]));
	CAN1->sTxMailBox[mbox].TIR|=1<<0; 	//请求发送邮箱数据
	return mbox;
}
//获得发送状态.
//mbox:邮箱编号;
//返回值:发送状态. 0,挂起;0X05,发送失败;0X07,发送成功.
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
			sta=0X05;//邮箱号不对,肯定失败.
		break;
	}
	return sta;
} 
//得到在FIFO0/FIFO1中接收到的报文个数.
//fifox:0/1.FIFO编号;
//返回值:FIFO0/FIFO1中的报文个数.
uint8_t CAN_Msg_Pend(uint8_t fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}
//接收数据
//fifox:邮箱号
//id:标准ID(11位)/扩展ID(11位+18位)	    
//ide:0,标准帧;1,扩展帧
//rtr:0,数据帧;1,远程帧
//len:接收到的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
//dat:数据缓存区
void CAN_Rx_Msg(uint8_t fifox,uint32_t *id,uint8_t *ide,uint8_t *rtr,uint8_t *len,uint8_t *dat)
{	   
	*ide=CAN1->sFIFOMailBox[fifox].RIR&0x04;	//得到标识符选择位的值  
 	if(*ide==0)//标准标识符
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>21;
	}else	   //扩展标识符
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>3;
	}
	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;	//得到远程发送请求值.
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;	//得到DLC
 	//*fmi=(CAN1->sFIFOMailBox[FIFONumber].RDTR>>8)&0xFF;//得到FMI
	//接收数据
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN1->RF0R|=0X20;//释放FIFO0邮箱
	else if(fifox==1)CAN1->RF1R|=0X20;//释放FIFO1邮箱	 
}

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
uint8_t CAN_Send_Msg(uint8_t* msg,uint8_t len)
{	
	uint8_t mbox;
	uint16_t i=0;	  	 						       
    mbox=CAN_Tx_Msg(0X12,0,0,len,msg);
	while((CAN_Tx_Staus(mbox)!=0X07)&&(i<0XFFF))i++;//等待发送结束
	if(i>=0XFFF)return 1;							//发送失败?
	return 0;										//发送成功;
}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
uint8_t CAN_Receive_Msg(uint8_t *buf)
{		   		   
	uint32_t id;
	uint8_t ide,rtr,len; 
	if(CAN_Msg_Pend(0)==0)return 0;			//没有接收到数据,直接退出 	 
  	CAN_Rx_Msg(0,&id,&ide,&rtr,&len,buf); 	//读取数据
    if(id!=0x12||ide!=0||rtr!=0)len=0;		//接收错误	   
	return len;	
}

/***************************************************************************************/

CAN_RecvTypeDef CAN_RecvInf; 
// CAN_FIFO0 0 
#if CAN_RX0_INT_ENABLE      	//使能RX0中断
//中断服务函数			  
void CAN1_RX0_IRQHandler(void)
{
	  CAN_RecvInf.CRecvFlag0=1;
	  CAN_Rx_Msg(0,&CAN_RecvInf.RecvMessage0.StdId,&CAN_RecvInf.RecvMessage0.IDE,
		&CAN_RecvInf.RecvMessage0.RTR,&CAN_RecvInf.RecvMessage0.DLC,CAN_RecvInf.RecvMessage0.Data);
//	  NVIC_DisableIRQ(CAN1_RX0_IRQn);
	  #if 1 
		printf("Rx ID：0x%x \r\n",CAN_RecvInf.RecvMessage0.StdId);
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
//ide:0,标准帧;1,扩展帧
//rtr:0,数据帧;1,远程帧
	uint8_t mbox;
    mbox=CAN_Tx_Msg(id,0,0,len,p);
	while(1)
	{
		if(CAN_Tx_Staus(mbox)!=0X07)//判断状态
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
/*控制命令解析*/
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
//函数: CAN接收到信息后，进行外部扫描适配
//输入：CanRxMsg
//输出：对应的全局变量
//注释: 
//作  者：lyn
uint8_t CANScan(void)
{

	if(CAN_RecvInf.CRecvFlag0)
	{
		for(uint8_t i=0;i<sizeof(CANAnalyzTab)/sizeof(CANAnalyzTypeDef);i++)
		{						 
			 if(CAN_RecvInf.RecvMessage0.StdId == CANAnalyzTab[i].CAN_ID)//鉴别EMS_EngineID 转速 电压等信息
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
	p1->ctrl_tim_one = 	Tim_Cnt[0].count ==0? 0:Tim_Cnt[0].count/60+1 ; //单位 分钟
//	printf("tim min:%d.%d\n",Tim_Cnt[0].count,Tim_Cnt[0].count/60+1);
	p1->ctrl_sta_two = CTRL_TWO_GPIO_Port->IDR & CTRL_TWO_Pin?true:false;
	p1->ctrl_tim_two = Tim_Cnt[1].count ==0? 0:Tim_Cnt[1].count/60+1 ; //单位 分钟;
	p1->ctrl_sta_three = CTRL_THREE_GPIO_Port->IDR & CTRL_THREE_Pin?true:false;
	p1->ctrl_tim_three = Tim_Cnt[3].count ==0? 0:Tim_Cnt[2].count/60+1 ; //单位 分钟
	p1->ctrl_sta_four = CTRL_FOUR_GPIO_Port->IDR & CTRL_FOUR_Pin ?true:false;
	p1->ctrl_tim_four = Tim_Cnt[4].count ==0? 0:Tim_Cnt[4].count/60+1 ; //单位 分钟
	p1->ctrl_sta_five = CTRL_FIVE_GPIO_Port->IDR & CTRL_FIVE_Pin?true:false;
	p1->ctrl_tim_five = Tim_Cnt[5].count ==0? 0:Tim_Cnt[5].count/60+1 ; //单位 分钟
	p1->ctrl_sta_six = CTRL_SIX_GPIO_Port->IDR & CTRL_SIX_Pin?true:false;
	p1->ctrl_tim_six = Tim_Cnt[6].count ==0? 0:Tim_Cnt[6].count/60+1 ; //单位 分钟
	p1->ctrl_sta_seven = CTRL_SEVEN_GPIO_Port->IDR & CTRL_SEVEN_Pin?true:false;
	p1->ctrl_tim_seven = Tim_Cnt[6].count ==0? 0:Tim_Cnt[6].count/60+1 ; //单位 分钟
	p1->ctrl_sta_eight = CTRL_EIGHT_GPIO_Port->IDR & CTRL_EIGHT_Pin?true:false;
	p1->ctrl_tim_eight = Tim_Cnt[7].count ==0? 0:Tim_Cnt[7].count/60+1 ; //单位 分钟
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
//函数功能：进行数据的CAN发送
//作  者：lyn
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
			return CAN_Send_Tab[i].CAN_Send_Fun;//返回值后面可以进行轮询调用 lyn
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


