#include "f28x_project.h"
#include "math.h"

#ifndef PWM_H_
#define PWM_H_

void initEPWM1(void);

void initEPWM2(void);

void initEPWM3(void);

void initEPWM5(void);

#pragma CODE_SECTION(Circuit1_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit2_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit3_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit4_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit5_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit6_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(FanDuty, ".TI.ramfunc");



void Circuit1_Duty(float duty);

void Circuit2_Duty(float duty);

void Circuit3_Duty(float duty);

void Circuit4_Duty(float duty);

void Circuit5_Duty(float duty);

void Circuit6_Duty(float duty);

void FanDuty(float duty);

#endif
