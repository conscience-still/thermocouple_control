#include "temp_ctrl.h"
#include "timer.h"
#include "stdio.h"
#include "can_self.h"
CANSend33B cansend33b_t_msg;

Ctrl_Sta_t CTRL_STA;
TimCntTypeDef Tim_Cnt[8];
CAN_Ctrl_TypeDef can_ctrl;
Ctrl_Temperature ctrl_temperature[8];
void Clear_Ctrl_Sta(uint8_t id)
{
	Tim_Cnt[id].status = 0;
	Tim_Cnt[id].start = 0;
}
void Set_Ctrl_Tim(uint8_t id,uint8_t tim_cnt)
{
	if(Tim_Cnt[id].flag ==0&&Tim_Cnt[id].start==0)
	{	
		printf("Tim_Cnt[%d].flag:%d\n",id,Tim_Cnt[id].flag);
		Tim_Cnt[id].flag =1;
		Tim_Cnt[id].src = tim_cnt;// 100ms定时
		Tim_Cnt[id].start = 1;
	}
} 
void cansend33b_t_msg_process(void)
{
	cansend33b_t_msg.Temperature_one =  adc_value.Temperature_One_Value <1080 ? 0: (float)adc_value.Temperature_One_Value  *0.1151 - 124.3;
	cansend33b_t_msg.Temperature_Two =  adc_value.Temperature_Two_Value <1080 ? 0:(float)adc_value.Temperature_Two_Value *0.1151 - 124.3;
	cansend33b_t_msg.Temperature_Three =  adc_value.Temperature_Three_Value <1080 ? 0:(float)adc_value.Temperature_Three_Value *0.1151 - 124.3;
	cansend33b_t_msg.Temperature_Four =  adc_value.Temperature_Four_Value <1080 ? 0:(float)adc_value.Temperature_Four_Value *0.1151 - 124.3;
	cansend33b_t_msg.Temperature_Five =  adc_value.Temperature_Five_Value <1080 ? 0:(float)adc_value.Temperature_Five_Value  *0.1151 - 124.3;
	cansend33b_t_msg.Temperature_Six =  adc_value.Temperature_Six_Value <1080 ? 0:(float)adc_value.Temperature_Six_Value *0.1151 - 124.3;
	cansend33b_t_msg.Temperature_Seven =  adc_value.Temperature_Seven_Value <1080 ? 0:(float)adc_value.Temperature_Seven_Value *0.1151 - 124.3;
	cansend33b_t_msg.Temperature_Eight =  adc_value.Temperature_Eight_Value <1080 ? 0:(float)adc_value.Temperature_Eight_Value *0.1151 - 124.3;

}
void Ctrl_ONE(uint8_t id)
{//默认到500
	float temp =adc_value.Temperature_One_Value <1080 ? 0: (float)adc_value.Temperature_One_Value  *0.1151 - 124.3;

//	printf("temp1:%f.%d.\n",temp,adc_value.Temperature_One_Value);
//	printf("temp2:%f.%d.\n",temp,adc_value.Temperature_Two_Value);
//	printf("temp3:%f.%d.\n",temp,adc_value.Temperature_Three_Value);
//	printf("temp4:%f.%d.\n",temp,adc_value.Temperature_Four_Value);
//	printf("temp5:%f.%d.\n",temp,adc_value.Temperature_Five_Value);
//	printf("temp6:%f.%d.\n",temp,adc_value.Temperature_Six_Value);
//	printf("temp7:%f.%d.\n",temp,adc_value.Temperature_Seven_Value);
//	printf("temp8:%f.%d.\n",temp,adc_value.Temperature_Eight_Value);
	static uint8_t tem_sta =0;
	if(temp < 120)
	{
		if(tem_sta == 0)
		{
			HAL_GPIO_WritePin(CTRL_ONE_GPIO_Port, CTRL_ONE_Pin, GPIO_PIN_SET);
		}
		if(temp < 120)tem_sta=0;
	}
	else
	{
		HAL_GPIO_WritePin(CTRL_ONE_GPIO_Port, CTRL_ONE_Pin, GPIO_PIN_RESET);
		tem_sta = 1;		
	}
	if(temp >120)
	{		
		
		Set_Ctrl_Tim(id,120);	//120s 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10
	}	
}
void Ctrl_TWO(uint8_t id)
{
	float temp =adc_value.Temperature_Two_Value <1080 ? 0:(float)adc_value.Temperature_Two_Value *0.1151 - 124.3;
//	printf("temp2:%f.%d.\n",temp,adc_value.Temperature_Two_Value);
	
	static uint8_t tem_sta =0;
	if(temp < 120)
	{
		if(tem_sta == 0)
		{
			HAL_GPIO_WritePin(CTRL_TWO_GPIO_Port, CTRL_TWO_Pin, GPIO_PIN_SET);
		}
		if(temp < 110)tem_sta=0;
	}
	else
	{
		HAL_GPIO_WritePin(CTRL_TWO_GPIO_Port, CTRL_TWO_Pin, GPIO_PIN_RESET);
		tem_sta = 1;		
	}	
	if(temp >120)
	{		
	  Set_Ctrl_Tim(id,180);	// 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10
	
	}		
	
} 
void Ctrl_THREE(uint8_t id)
{
	float temp =adc_value.Temperature_Three_Value <1080 ? 0:(float)adc_value.Temperature_Three_Value *0.1151 - 124.3;
	
	static uint8_t tem_sta =0;
	if(temp < 150)
	{
		if(tem_sta == 0)
		{
			HAL_GPIO_WritePin(CTRL_THREE_GPIO_Port, CTRL_THREE_Pin, GPIO_PIN_SET);
		}
		if(temp < 130)tem_sta=0;
	}
	else
	{
		HAL_GPIO_WritePin(CTRL_THREE_GPIO_Port, CTRL_THREE_Pin, GPIO_PIN_RESET);
		tem_sta = 1;		
	}

	if(temp >150)
	{		
		  Set_Ctrl_Tim(id,180);	// 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10
	}		
	
}
/*第四部分 第一路*/
void Ctrl_FOUR(uint8_t id)
{
	float temp =adc_value.Temperature_Four_Value <1080 ? 0:(float)adc_value.Temperature_Four_Value *0.1151 - 124.3;
	
	static uint8_t tem_sta =0;
	if(temp < 100)
	{
		if(tem_sta == 0)
		{
			HAL_GPIO_WritePin(CTRL_FOUR_GPIO_Port, CTRL_FOUR_Pin, GPIO_PIN_SET);
		}
		if(temp < 85)tem_sta=0;
	}
	else
	{
		HAL_GPIO_WritePin(CTRL_FOUR_GPIO_Port, CTRL_FOUR_Pin, GPIO_PIN_RESET);
		tem_sta = 1;		
	}
	if(temp >100)
	{		
	  Set_Ctrl_Tim(id,180);	// 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10	
	}		
	
}
/*第四部分 第二路*/
void Ctrl_FIVE(uint8_t id)
{
	float temp =adc_value.Temperature_Five_Value <1080 ? 0:(float)adc_value.Temperature_Five_Value  *0.1151 - 124.3;
	float temp1 =adc_value.Temperature_Four_Value <1080 ? 0:(float)adc_value.Temperature_Four_Value *0.1151 - 124.3;
	
	static uint8_t tem_sta =0;
	if(temp1 <30 &&temp < 100)
	{
		if(tem_sta == 0)
		{
			HAL_GPIO_WritePin(CTRL_FIVE_GPIO_Port, CTRL_FIVE_Pin, GPIO_PIN_SET);
		}
		if(temp < 85)tem_sta=0;
	}
	else
	{
		HAL_GPIO_WritePin(CTRL_FIVE_GPIO_Port, CTRL_FIVE_Pin, GPIO_PIN_RESET);
		tem_sta = 1;		
	}
	if(temp >100)
	{		
	  Set_Ctrl_Tim(id,180);	// 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10	
	}		
	
}
/*第四部分 第三路*/
void Ctrl_SIX(uint8_t id)
{
	float temp =adc_value.Temperature_Six_Value <1080 ? 0:(float)adc_value.Temperature_Six_Value *0.1151 - 124.3;
	float temp1 =adc_value.Temperature_Five_Value <1080 ? 0:(float)adc_value.Temperature_Five_Value  *0.1151 - 124.3;

	static uint8_t tem_sta =0;
	if(temp1 <30 &&temp < 100)
	{
		if(tem_sta == 0)
		{
			HAL_GPIO_WritePin(CTRL_SIX_GPIO_Port, CTRL_SIX_Pin, GPIO_PIN_SET);
		}
		if(temp < 85)tem_sta=0;
	}
	else
	{
		HAL_GPIO_WritePin(CTRL_SIX_GPIO_Port, CTRL_SIX_Pin, GPIO_PIN_RESET);
		tem_sta = 1;		
	}
	if(temp >100)
	{		
	  Set_Ctrl_Tim(id,180);	// 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10
	}		
	
}
/*第四部分 第四路*/
void Ctrl_SEVEN(uint8_t id)
{
	float temp =adc_value.Temperature_Seven_Value <1080 ? 0:(float)adc_value.Temperature_Seven_Value *0.1151 - 124.3;
	float temp1 =adc_value.Temperature_Six_Value <1080 ? 0:(float)adc_value.Temperature_Six_Value *0.1151 - 124.3;
	

	static uint8_t tem_sta =0;
	if(temp1 <30 &&temp < 130)
	{
		if(tem_sta == 0)
		{
			HAL_GPIO_WritePin(CTRL_SEVEN_GPIO_Port, CTRL_SEVEN_Pin, GPIO_PIN_SET);
		}
		if(temp < 115)tem_sta=0;
	}
	else
	{
		HAL_GPIO_WritePin(CTRL_SEVEN_GPIO_Port, CTRL_SEVEN_Pin, GPIO_PIN_RESET);
		tem_sta = 1;		
	}
	if(temp >120)
	{		
	  Set_Ctrl_Tim(id,180);	// 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10	
	}		
	
}
/*第五部分*/
void Ctrl_EIGHT(uint8_t id)
{
	float temp =adc_value.Temperature_Eight_Value <1080 ? 0:(float)adc_value.Temperature_Eight_Value *0.1151 - 124.3;
	float temp1 =adc_value.Temperature_Seven_Value <1080 ? 0:(float)adc_value.Temperature_Seven_Value *0.1151 - 124.3;
	
	static uint8_t tem_sta =0;
	if(temp1 <30 &&temp < 130)
	{
		if(tem_sta == 0)
		{
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_SET);
		}
		if(temp < 115)tem_sta=0;
	}
	else
	{
		HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);
		tem_sta = 1;		
	}
	if(temp >120)
	{		
	  Set_Ctrl_Tim(id,180);	// 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10	
	}	
	
}
void Tem_Ctrl_Time(void)
{
	for(char i=0;i<sizeof(Tim_Cnt)/ sizeof(TimCntTypeDef);i++)
	{
		if(Tim_Cnt[i].flag == 1)
		{	
				(Tim_Cnt[i].count/10 < Tim_Cnt[i].src)?(Tim_Cnt[i].count ++):(Tim_Cnt[i].count=0,Tim_Cnt[i].flag=0,Tim_Cnt[i].status++);/*定时器周期 1s*/
		}
	}
}


void Ctrl_test(uint8_t id)
{
	  Set_Ctrl_Tim(id,180);	// 其中 在定时器 Tem_Ctrl_Time 中count计数会进行除以10	
}
void Temperature_Construct(void)
{
	ctrl_temperature[0] = Ctrl_ONE;
	ctrl_temperature[1] = Ctrl_TWO;
	ctrl_temperature[2] = Ctrl_THREE;
	ctrl_temperature[3] = Ctrl_FOUR;
	ctrl_temperature[4] = Ctrl_FIVE;
	ctrl_temperature[5] = Ctrl_SIX;
	ctrl_temperature[6] = Ctrl_SEVEN;
	ctrl_temperature[7] = Ctrl_EIGHT;
}

void Close_Other_Ctrl(uint8_t id)
{
	switch(id)
	{
			case FIRST_STA:
			HAL_GPIO_WritePin(GPIOB, CTRL_TWO_Pin|CTRL_THREE_Pin|CTRL_FOUR_Pin
                          |CTRL_FIVE_Pin|CTRL_SEVEN_Pin, GPIO_PIN_RESET);
			/*Configure GPIO pin Output Level */
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);
				break;	
			case SECOND_STA:
			HAL_GPIO_WritePin(GPIOB, CTRL_ONE_Pin|CTRL_THREE_Pin|CTRL_FOUR_Pin
                          |CTRL_FIVE_Pin|CTRL_SEVEN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);
				break;	
			
			case THIRD_STA:
			HAL_GPIO_WritePin(GPIOB, CTRL_ONE_Pin|CTRL_TWO_Pin|CTRL_FOUR_Pin
                          |CTRL_FIVE_Pin|CTRL_SEVEN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);
			

				break;	
			case FOURTH_STA:
			HAL_GPIO_WritePin(GPIOB, CTRL_ONE_Pin|CTRL_TWO_Pin|CTRL_THREE_Pin
                          |CTRL_FIVE_Pin|CTRL_SEVEN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);				
				break;	

			case FIFTH_STA:
			HAL_GPIO_WritePin(GPIOB, CTRL_ONE_Pin|CTRL_TWO_Pin|CTRL_THREE_Pin|CTRL_FOUR_Pin
                          |CTRL_SEVEN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);				
				break;	

			case SIXTH_STA:
			HAL_GPIO_WritePin(GPIOB, CTRL_ONE_Pin|CTRL_TWO_Pin|CTRL_THREE_Pin|CTRL_FOUR_Pin
                          |CTRL_FIVE_Pin|CTRL_SEVEN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);				
				break;	

			case SEVENTH_STA:
			HAL_GPIO_WritePin(GPIOB, CTRL_ONE_Pin|CTRL_TWO_Pin|CTRL_THREE_Pin|CTRL_FOUR_Pin
                          |CTRL_FIVE_Pin|CTRL_SIX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);				
				break;	
			case EIGHTTH_STA: 
			HAL_GPIO_WritePin(GPIOB, CTRL_ONE_Pin|CTRL_TWO_Pin|CTRL_THREE_Pin|CTRL_FOUR_Pin
                          |CTRL_FIVE_Pin|CTRL_SIX_Pin|CTRL_SEVEN_Pin, GPIO_PIN_RESET);
				break;					
			default:
			HAL_GPIO_WritePin(GPIOB, CTRL_ONE_Pin|CTRL_TWO_Pin|CTRL_THREE_Pin|CTRL_FOUR_Pin
                          |CTRL_FIVE_Pin|CTRL_SIX_Pin|CTRL_SEVEN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(CTRL_EIGHT_GPIO_Port, CTRL_EIGHT_Pin, GPIO_PIN_RESET);				
				printf("close all \n");
				break;	
	}
}
void Temperature_Process(Ctrl_Sta_t sta)
{
	cansend33b_t_msg_process();
	if(can_ctrl.ctrl_num == 0xff)
	{
//		printf("CTRL_STA:%d.%d\r\n",CTRL_STA,can_ctrl.ctrl_num);
		switch(sta)
		{
			case FIRST_STA:
			case SECOND_STA:
			case THIRD_STA:
			case FOURTH_STA:
			case FIFTH_STA:
			case SIXTH_STA:
			case SEVENTH_STA:
			case EIGHTTH_STA:
				ctrl_temperature[CTRL_STA](CTRL_STA);
				Close_Other_Ctrl(CTRL_STA);

				if(Tim_Cnt[CTRL_STA].status)
				{		
					Clear_Ctrl_Sta(CTRL_STA);				
					CTRL_STA++;
				}
				break;

			default:
				Close_Other_Ctrl(0xff);
//				printf("default sta\n");
				break;
		}
	}
	else if(can_ctrl.ctrl_num > 0)
	{
		Close_Other_Ctrl(can_ctrl.ctrl_num-1);//因为 控制的数字是从1开始 而执行case是从0开始  所以相差一个1
		
		if(Tim_Cnt[can_ctrl.ctrl_num-1].status == 0)
		{		
			ctrl_temperature[can_ctrl.ctrl_num-1](can_ctrl.ctrl_num-1);
		}		
		else
		{
			Close_Other_Ctrl(0xff);
			printf("close can_ctrl.ctrl_num \n");
		}
	}		
//	else
//	{
//		printf("no ctrl \n");
//	}
}


