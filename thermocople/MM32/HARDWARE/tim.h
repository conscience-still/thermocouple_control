#ifndef __TIM_H_
#define __TIM_H_
#include "HAL_conf.h"
#ifdef __cplusplus
extern "C"{
#endif
void Tim2_UPCount_test(u16 Prescaler,u16 Period);
void Tim2_UPStatusOVCheck_test(void);
extern unsigned int ucTim2Flag;
#ifdef __cplusplus
}
#endif
#endif
