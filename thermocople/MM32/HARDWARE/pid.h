#ifndef _PID_H
#define _PID_H

#include "HAL_conf.h"


/*----------PID------------------------*/

typedef struct 
{
    float _kp;
    float _ki;
    float _kd;
    float _iter;
    float _imax;
    float _imin;
    float _omax;
    float _omin;
    // _alph  = 2*pi*f*t;
    float _alph;//lowpass for d
    float _lastd[3];
}PID_typedef;
#ifdef __cplusplus
extern "C" {
#endif
//const delta time
void SetPidPra(PID_typedef* pid, float kp,float ki,float kd);
float GetPid(PID_typedef* pid,float m,float ep);
#ifdef __cplusplus
}
#endif
#endif
