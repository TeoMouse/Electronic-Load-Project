#include "adc.h"
#define SAMPLE_WIN      (40)
#define SAMPLE_WIN2     (100)

#define TEMP_C12        (0.00003f)
#define TEMP_C11        (-0.0994f)
#define TEMP_C10        (57.583f)

#define TEMP_C22        (0.006f)
#define TEMP_C21        (-1.4777f)
#define TEMP_C20        (148.32f)

void ConfigureADC(void){

    EALLOW;

    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFASEL = 1; // Reference voltage of the ADCA is set external
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFCSEL = 1; // Reference voltage of the ADCC is set external


    AdcaRegs.ADCCTL2.bit.PRESCALE = 4; //set ADCCLK divider to /2

    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1; //interrupt pulse generation occurs at the end of the conversion
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1; //power up ADCA

    DELAY_US(1000);

    AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4

    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1; //interrupt pulse generation occurs at the end of the conversion
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1; //power up ADCC


    EDIS;

    DELAY_US(1000); //delay for 1ms to allow ADC time to power up


}

void SetupADC(void){

    EALLOW;

    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin A0  /*I_C6*/
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = SAMPLE_WIN; //sample window is 9+1 sysclocks
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 1; //SOC0 conversion trigger -> Timer0

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC1 will convert pin A1 /*I_C5*/
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = SAMPLE_WIN; //sample window is 9+1 sysclocks
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 1; //SOC1 conversion trigger -> Timer0

    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC2 will convert pin A2 /*I_C3*/
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = SAMPLE_WIN; //sample window is 9+1 sysclocks
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 1; //SOC2 conversion trigger -> Timer0

    AdcaRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC3 will convert pin A5 /*I_C2*/
    AdcaRegs.ADCSOC3CTL.bit.ACQPS = SAMPLE_WIN; //sample window is 9+1 sysclocks
    AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 1; //SOC3 conversion trigger -> Timer0

    AdcaRegs.ADCSOC4CTL.bit.CHSEL = 5;  //SOC4 will convert pin A5 /*I_C4*/
    AdcaRegs.ADCSOC4CTL.bit.ACQPS = SAMPLE_WIN; //sample window is 9+1 sysclocks
    AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = 1; //SOC4 conversion trigger -> Timer0

    AdcaRegs.ADCSOC5CTL.bit.CHSEL = 6;  //SOC5 will convert pin A6 /*I_C1*/
    AdcaRegs.ADCSOC5CTL.bit.ACQPS = SAMPLE_WIN; //sample window is 9+1 sysclocks
    AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = 1; //SOC5 conversion trigger -> Timer0

    AdcaRegs.ADCSOC6CTL.bit.CHSEL = 12;  //SOC6 will convert pin A12 /*Vin_Meas*/
    AdcaRegs.ADCSOC6CTL.bit.ACQPS = SAMPLE_WIN; //sample window is 9+1 sysclocks
    AdcaRegs.ADCSOC6CTL.bit.TRIGSEL = 1; //SOC6 conversion trigger -> Timer0

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 6; //EOC6 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

    //ADC_C Temperature measurements

    AdccRegs.ADCSOC0CTL.bit.CHSEL = 3;  //SOC0 will convert pin C3 /*Temp1*/
    AdccRegs.ADCSOC0CTL.bit.ACQPS = SAMPLE_WIN2; //sample window is 9+1 sysclocks
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 2; //SOC0 conversion trigger -> Timer1

    AdccRegs.ADCSOC1CTL.bit.CHSEL = 8;  //SOC1 will convert pin C8 /*Temp2*/
    AdccRegs.ADCSOC1CTL.bit.ACQPS = SAMPLE_WIN2; //sample window is 9+1 sysclocks
    AdccRegs.ADCSOC1CTL.bit.TRIGSEL = 2; //SOC1 conversion trigger -> Timer1

    AdccRegs.ADCSOC2CTL.bit.CHSEL = 10;  //SOC2 will convert pin C10 /*Potentiometer*/
    AdccRegs.ADCSOC2CTL.bit.ACQPS = SAMPLE_WIN2; //sample window is 9+1 sysclocks
    AdccRegs.ADCSOC2CTL.bit.TRIGSEL = 2; //SOC2 conversion trigger -> Timer1

    AdccRegs.ADCSOC3CTL.bit.CHSEL = 11;  //SOC3 will convert pin C11 /*Temp4*/
    AdccRegs.ADCSOC3CTL.bit.ACQPS = SAMPLE_WIN2; //sample window is 9+1 sysclocks
    AdccRegs.ADCSOC3CTL.bit.TRIGSEL = 2; //SOC3 conversion trigger -> Timer1

    AdccRegs.ADCSOC4CTL.bit.CHSEL = 14;  //SOC3 will convert pin C14 /*Temp3*/
    AdccRegs.ADCSOC4CTL.bit.ACQPS = SAMPLE_WIN2; //sample window is 9+1 sysclocks
    AdccRegs.ADCSOC4CTL.bit.TRIGSEL = 2; //SOC3 conversion trigger -> Timer1

    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 4; //EOC3 will set INT1 flag
    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared


    EDIS;
}

/* Execute the functions to get ADC values from RAM */
#pragma CODE_SECTION(GetVin, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir1, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir2, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir3, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir4, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir5, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir6, ".TI.ramfunc");


float GetVin(void){
    float Val = 0;

    Val = 0.01465f * AdcaResultRegs.ADCRESULT6;

    return Val;
}

float Get_Icir1(void){
    float Val = 0;

    Val = 0.00131f * AdcaResultRegs.ADCRESULT5;

    return Val;

}

float Get_Icir2(void){
    float Val = 0;

    Val = 0.00131f * AdcaResultRegs.ADCRESULT3;

    return Val;

}

float Get_Icir3(void){
    float Val = 0;

    Val = 0.00131f * AdcaResultRegs.ADCRESULT2;

    return Val;

}

float Get_Icir4(void){
    float Val = 0;

    Val = 0.00131f * AdcaResultRegs.ADCRESULT4;

    return Val;

}

float Get_Icir5(void){
    float Val = 0;

    Val = 0.00131f * AdcaResultRegs.ADCRESULT1;

    return Val;

}

float Get_Icir6(void){
    float Val = 0;

    Val = 0.00131f * AdcaResultRegs.ADCRESULT0;

    return Val;

}

#pragma CODE_SECTION(GetTemp1, ".TI.ramfunc");
#pragma CODE_SECTION(GetTemp2, ".TI.ramfunc");
#pragma CODE_SECTION(GetTemp3, ".TI.ramfunc");
#pragma CODE_SECTION(GetTemp4, ".TI.ramfunc");


float GetTemp1(void){

    float Val = 0;
    uint16_t ADC_Res = AdccResultRegs.ADCRESULT0;

    if ( ADC_Res > 100){
        Val = TEMP_C12*ADC_Res*ADC_Res + TEMP_C11*ADC_Res+ TEMP_C10;
    }
    else{
        Val = TEMP_C22*ADC_Res*ADC_Res + TEMP_C21*ADC_Res+ TEMP_C20;
    }

    return Val;
}

float GetTemp2(void){

    float Val = 0;
    uint16_t ADC_Res = AdccResultRegs.ADCRESULT1;

    if ( ADC_Res > 100){
        Val = TEMP_C12*ADC_Res*ADC_Res + TEMP_C11*ADC_Res+ TEMP_C10;
    }
    else{
        Val = TEMP_C22*ADC_Res*ADC_Res + TEMP_C21*ADC_Res+ TEMP_C20;
    }

    return Val;
}

float GetTemp3(void){

    float Val = 0;
    uint16_t ADC_Res = AdccResultRegs.ADCRESULT4;

    if ( ADC_Res > 100){
        Val = TEMP_C12*ADC_Res*ADC_Res + TEMP_C11*ADC_Res+ TEMP_C10;
    }
    else{
        Val = TEMP_C22*ADC_Res*ADC_Res + TEMP_C21*ADC_Res+ TEMP_C20;
    }

    return Val;
}

float GetTemp4(void){

    float Val = 0;
    uint16_t ADC_Res = AdccResultRegs.ADCRESULT3;

    if ( ADC_Res > 100){
        Val = TEMP_C12*ADC_Res*ADC_Res + TEMP_C11*ADC_Res+ TEMP_C10;
    }
    else{
        Val = TEMP_C22*ADC_Res*ADC_Res + TEMP_C21*ADC_Res+ TEMP_C20;
    }

    return Val;
}


uint16_t GetPotValue(void){

    uint16_t ADC = AdccResultRegs.ADCRESULT2;

    return ADC;

}

#pragma CODE_SECTION(shift, ".TI.ramfunc");
#pragma CODE_SECTION(CLR_Array, ".TI.ramfunc");
#pragma CODE_SECTION(Array_Set, ".TI.ramfunc");
#pragma CODE_SECTION(secfilt, ".TI.ramfunc");


void shift(float *array, int length){

    int i;
    for (i = length;i > 1;i--) array[i-1] = array[i-2];

    return;
}

void CLR_Array(float *array, int length){

    int i;
    for (i = 0;i < length;i++) array[i] = 0;
}

void Array_Set(float *array, int length, float value){

    int i;
    for (i = 0;i < length;i++) array[i] = value;
}

/* 2nd Order Filter Section */
void secfilt(float *in, float *y, float *a,float *b, float g){

    y[0] = b[0] * in[0]*g;
    y[0] += b[1] * in[1]*g;
    y[0] += b[2] * in[2]*g;
    y[0] -= a[1] * y[1];
    y[0] -= a[2] * y[2];

}

