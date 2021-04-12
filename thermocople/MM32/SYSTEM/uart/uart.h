#ifndef __UART_H
#define __UART_H
#include "HAL_conf.h"
#include  "stdio.h"
#include "dtype.h"
		   
#ifdef __cplusplus
extern "C"{
#endif
//开发板
//串口1初始化
void uart_initwBaudRate(u32 bound);
#ifdef __cplusplus
}
#endif
#endif
	


