#ifndef __CAN_H
#define __CAN_H
#include "main.h"

//#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//CAN��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/9/18
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//CAN1����RX0�ж�ʹ��
#define CAN1_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.

uint8_t CAN1_Mode_Init(uint32_t tsjw,uint32_t tbs2,uint32_t tbs1,uint16_t brp,uint32_t mode);//CAN��ʼ��
void CAN_Config(void);
uint8_t CAN1_Send_Msg(uint8_t* msg,uint8_t len);						//��������
uint8_t CAN1_Receive_Msg(uint8_t *buf);							//��������
#endif
