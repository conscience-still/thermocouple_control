#include "temprate.h"

void temprate_Init(void)
{
    
   GPIO_InitTypeDef  GPIO_InitStructure;
 	
    
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);  //¿ªÆôGPIOA,GPIOBÊ±ÖÓ

	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
}








