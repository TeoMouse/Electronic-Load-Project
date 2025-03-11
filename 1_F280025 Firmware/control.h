#include "f28x_project.h"
#include "math.h"

#ifndef CONTROL_H_
#define CONTROL_H_

void InitLEDGpio(void);

#pragma CODE_SECTION(DutyCalc, ".TI.ramfunc");
#pragma CODE_SECTION(PIcontroller, ".TI.ramfunc");
#pragma CODE_SECTION(saturator, ".TI.ramfunc");
#pragma CODE_SECTION(AntiwindRST, ".TI.ramfunc");

float DutyCalc(float ILoad_Div);

void LED2_ON(void);

void LED2_OFF(void);


void PIcontroller(float *OutputArray, float *ErrorArray, float kp, float ki, float Ts);


float saturator(float min, float max, float value);


uint16_t AntiwindRST(float Out, float OutSat, float Err);

#endif
