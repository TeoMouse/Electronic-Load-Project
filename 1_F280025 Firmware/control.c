#include "control.h"


#define R_SHUNT        (100) //100 mOhms Rshunt
#define DIV_CON        (6.555f) //5A maximum current per device if duty = 1
#define VGPIO_PK       (3300) //3300 mV


void InitLEDGpio(void){
    EALLOW;

    //GPIO33 Configurations (Output) MCU_LED2
    GpioCtrlRegs.GPBGMUX1.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0; // Pin MUXING: Configure GPIO33 as GPIO
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 1; // Disable Pull-Up
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1; //GPIO33 is output


    EDIS;

}

#pragma CODE_SECTION(DutyCalc, ".TI.ramfunc");
#pragma CODE_SECTION(PIcontroller, ".TI.ramfunc");
#pragma CODE_SECTION(saturator, ".TI.ramfunc");
#pragma CODE_SECTION(AntiwindRST, ".TI.ramfunc");

float DutyCalc(float ILoad_Div){

    float duty;
    duty = ILoad_Div*R_SHUNT*DIV_CON/VGPIO_PK;

    return duty;
}

void LED2_ON(void){
    GpioDataRegs.GPBSET.bit.GPIO33 = 1;
}

void LED2_OFF(void){
    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;
}




void PIcontroller(float *OutputArray, float *ErrorArray, float kp, float ki, float Ts){

    OutputArray[0] = (kp+ki*Ts)*ErrorArray[0] -kp*ErrorArray[1] + OutputArray[1];
}



float saturator(float min, float max, float value){

    if (value >= max) value = max;
    if (value <= min) value = min;

    return value;
}



uint16_t AntiwindRST(float Out, float OutSat, float Err){
    int16_t SignErr=0, SignOut=0, EqOut = 0, EqSign = 0, value = 0 ;
    if ((Out-OutSat) < 0.001f || (Out-OutSat) < 0.001f){
        EqOut = 1;
    }
    else{
        EqOut = 0;
    }

    if(Out>0){
        SignOut = 1;
    }
    else{
        SignOut = -1;
    }

    if(Err>0){
        SignErr = 1;
    }
    else{
        SignErr = -1;
    }

    if (SignErr == SignOut){
        EqSign = 1;
    }
    else{
        EqSign = 0;
    }

    if (EqOut == 0 && EqSign == 1){
        value = 1;
    }
    else{
        value = 0;
    }

    return value;
}

