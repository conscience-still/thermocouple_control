#ifndef _TEMP_CTRL_H
#define _TEMP_CTRL_H
#include "main.h"
/*本文件主要定义控制温度函数*/

typedef enum{
	FIRST_STA =0 ,
	SECOND_STA ,
	THIRD_STA ,
	FOURTH_STA ,
	FIFTH_STA ,
	SIXTH_STA ,
	SEVENTH_STA ,
	EIGHTTH_STA	,
}Ctrl_Sta_t;
typedef struct __attribute__((__packed__)) 
{
	uint8_t  start:1;
	uint8_t  status:7;
	uint16_t src;
	uint16_t count;
	uint8_t  flag;
}TimCntTypeDef;
typedef struct 
{
	uint8_t  ctrl_num;
	uint8_t  ctrl_tim;
}CAN_Ctrl_TypeDef;
void Tem_Ctrl_Time(void);
void Temperature_Construct(void);
extern CAN_Ctrl_TypeDef can_ctrl;
typedef void(*Ctrl_Temperature)(uint8_t);
extern TimCntTypeDef Tim_Cnt[8];
extern Ctrl_Sta_t CTRL_STA;
void Temperature_Process(Ctrl_Sta_t sta);
void Clear_Ctrl_Sta(uint8_t id);
#endif

