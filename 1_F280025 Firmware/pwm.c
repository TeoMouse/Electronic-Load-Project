#include "pwm.h"

#define PWM_PERIOD      (2000) // fs = 50 kHz
#define DUTY_C          (1000)


void initEPWM1(void){

    EALLOW;
    EPwm1Regs.TZSEL.bit.OSHT1 = 1; //Enable TZ1 as a one-shot trip source for this ePWM module
    EPwm1Regs.TZCTL.bit.TZA = 2; //Force EPWMxA to a low state
    EPwm1Regs.TZCTL.bit.TZB = 2; //Force EPWMxB to a low state


    EPwm1Regs.TBCTR = 0; // Clear counter
    EPwm1Regs.TBPRD = PWM_PERIOD;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0; // HighSpeedTimeBaseClock Prescaler = /1
    EPwm1Regs.TBCTL.bit.CLKDIV = 0; // TimeBaseClock Prescaler = /1
    EPwm1Regs.TBPHS.bit.TBPHS = 0; // Phase is 0

    EPwm1Regs.CMPA.bit.CMPA = DUTY_C; // Set compare A value
    EPwm1Regs.CMPB.bit.CMPB = 0; // Set Compare B value

    EPwm1Regs.TBCTL.bit.CTRMODE = 0; // Up count mode
    EPwm1Regs.TBCTL.bit.PHSEN = 0; // Disable phase loading
    EPwm1Regs.TBCTL.bit.SWFSYNC = 1; // Force 1 time sync pulse to be generated

    EPwm1Regs.AQCTLA.bit.CAU = 1; // when CMPA == TBCTR on count up force output LOW
    EPwm1Regs.AQCTLA.bit.PRD = 2; // when PRD == TBCTR force output HIGH

    EPwm1Regs.AQCTLB.bit.CBU = 1; // when CMPB == TBCTR on count up force output LOW
    EPwm1Regs.AQCTLB.bit.PRD = 2; // when PRD == TBCTR force output HIGH

    EPwm1Regs.DBCTL.bit.OUT_MODE = 0; // Dead band is fully disabled
    EDIS;

}



void initEPWM2(void){

    EALLOW;
    EPwm2Regs.TZSEL.bit.OSHT1 = 1; //Enable TZ1 as a one-shot trip source for this ePWM module
    EPwm2Regs.TZCTL.bit.TZA = 2; //Force EPWMxA to a low state
    EPwm2Regs.TZCTL.bit.TZB = 2; //Force EPWMxB to a low state


    EPwm2Regs.TBCTR = 0; // Clear counter
    EPwm2Regs.TBPRD = PWM_PERIOD;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0; // HighSpeedTimeBaseClock Prescaler = /1
    EPwm2Regs.TBCTL.bit.CLKDIV = 0; // TimeBaseClock Prescaler = /1
    EPwm2Regs.TBPHS.bit.TBPHS = 0; // Phase is 0

    EPwm2Regs.CMPA.bit.CMPA = DUTY_C; // Set compare A value
    EPwm2Regs.CMPB.bit.CMPB = 0; // Set Compare B value

    EPwm2Regs.TBCTL.bit.CTRMODE = 0; // Up count mode
    EPwm2Regs.TBCTL.bit.PHSEN = 0; // Disable phase loading
    EPwm2Regs.TBCTL.bit.SWFSYNC = 1; // Force 1 time sync pulse to be generated

    EPwm2Regs.AQCTLA.bit.CAU = 1; // when CMPA == TBCTR on count up force output LOW
    EPwm2Regs.AQCTLA.bit.PRD = 2; // when PRD == TBCTR force output HIGH

    EPwm2Regs.AQCTLB.bit.CBU = 1; // when CMPB == TBCTR on count up force output LOW
    EPwm2Regs.AQCTLB.bit.PRD = 2; // when PRD == TBCTR force output HIGH

    EPwm2Regs.DBCTL.bit.OUT_MODE = 0; // Dead band is fully disabled
    EDIS;

}

void initEPWM3(void){

    EALLOW;
    EPwm3Regs.TZSEL.bit.OSHT1 = 1; //Enable TZ1 as a one-shot trip source for this ePWM module
    EPwm3Regs.TZCTL.bit.TZA = 1; //Force EPWMxA to a high state (FAN control)
    EPwm3Regs.TZCTL.bit.TZB = 2; //Force EPWMxB to a low state


    EPwm3Regs.TBCTR = 0; // Clear counter
    EPwm3Regs.TBPRD = PWM_PERIOD;
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0; // HighSpeedTimeBaseClock Prescaler = /1
    EPwm3Regs.TBCTL.bit.CLKDIV = 0; // TimeBaseClock Prescaler = /1
    EPwm3Regs.TBPHS.bit.TBPHS = 0; // Phase is 0

    EPwm3Regs.CMPA.bit.CMPA = DUTY_C; // Set compare A value
    EPwm3Regs.CMPB.bit.CMPB = 0; // Set Compare B value

    EPwm3Regs.TBCTL.bit.CTRMODE = 0; // Up count mode
    EPwm3Regs.TBCTL.bit.PHSEN = 0; // Disable phase loading
    EPwm3Regs.TBCTL.bit.SWFSYNC = 1; // Force 1 time sync pulse to be generated

    EPwm3Regs.AQCTLA.bit.CAU = 1; // when CMPA == TBCTR on count up force output LOW
    EPwm3Regs.AQCTLA.bit.PRD = 2; // when PRD == TBCTR force output HIGH

    EPwm3Regs.AQCTLB.bit.CBU = 1; // when CMPB == TBCTR on count up force output LOW
    EPwm3Regs.AQCTLB.bit.PRD = 2; // when PRD == TBCTR force output HIGH

    EPwm3Regs.DBCTL.bit.OUT_MODE = 0; // Dead band is fully disabled
    EDIS;
}


void initEPWM5(void){

    EALLOW;
    EPwm5Regs.TZSEL.bit.OSHT1 = 1; //Enable TZ1 as a one-shot trip source for this ePWM module
    EPwm5Regs.TZCTL.bit.TZA = 2; //Force EPWMxA to a low state
    EPwm5Regs.TZCTL.bit.TZB = 2; //Force EPWMxB to a low state


    EPwm5Regs.TBCTR = 0; // Clear counter
    EPwm5Regs.TBPRD = PWM_PERIOD;
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0; // HighSpeedTimeBaseClock Prescaler = /1
    EPwm5Regs.TBCTL.bit.CLKDIV = 0; // TimeBaseClock Prescaler = /1
    EPwm5Regs.TBPHS.bit.TBPHS = 0; // Phase is 0

    EPwm5Regs.CMPA.bit.CMPA = DUTY_C; // Set compare A value
    EPwm5Regs.CMPB.bit.CMPB = 0; // Set Compare B value

    EPwm5Regs.TBCTL.bit.CTRMODE = 0; // Up count mode
    EPwm5Regs.TBCTL.bit.PHSEN = 0; // Disable phase loading
    EPwm5Regs.TBCTL.bit.SWFSYNC = 1; // Force 1 time sync pulse to be generated

    EPwm5Regs.AQCTLA.bit.CAU = 1; // when CMPA == TBCTR on count up force output LOW
    EPwm5Regs.AQCTLA.bit.PRD = 2; // when PRD == TBCTR force output HIGH

    EPwm5Regs.DBCTL.bit.OUT_MODE = 0; // Dead band is fully disabled
    EDIS;

}

#pragma CODE_SECTION(Circuit1_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit2_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit3_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit4_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit5_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(Circuit6_Duty, ".TI.ramfunc");
#pragma CODE_SECTION(FanDuty, ".TI.ramfunc");


void Circuit1_Duty(float duty){

    EPwm2Regs.CMPB.bit.CMPB = duty*PWM_PERIOD;
}

void Circuit2_Duty(float duty){

    EPwm1Regs.CMPB.bit.CMPB = duty*PWM_PERIOD;
}

void Circuit3_Duty(float duty){

    EPwm2Regs.CMPA.bit.CMPA = duty*PWM_PERIOD;
}

void Circuit4_Duty(float duty){

    EPwm3Regs.CMPB.bit.CMPB = duty*PWM_PERIOD;
}

void Circuit5_Duty(float duty){

    EPwm5Regs.CMPA.bit.CMPA = duty*PWM_PERIOD;
}

void Circuit6_Duty(float duty){

    EPwm1Regs.CMPA.bit.CMPA = duty*PWM_PERIOD;
}

void FanDuty(float duty){

    EPwm3Regs.CMPA.bit.CMPA = duty*PWM_PERIOD;
}

