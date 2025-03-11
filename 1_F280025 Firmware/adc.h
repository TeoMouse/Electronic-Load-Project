
#include "f28x_project.h"
#include "math.h"

#ifndef ADC_H_
#define ADC_H_


void ConfigureADC(void);
void SetupADC(void);

/* Execute the functions to get ADC values from RAM */
#pragma CODE_SECTION(GetVin, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir1, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir2, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir3, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir4, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir5, ".TI.ramfunc");
#pragma CODE_SECTION(Get_Icir6, ".TI.ramfunc");

float GetVin(void);

float Get_Icir1(void);

float Get_Icir2(void);

float Get_Icir3(void);

float Get_Icir4(void);

float Get_Icir5(void);

float Get_Icir6(void);

#pragma CODE_SECTION(GetTemp1, ".TI.ramfunc");
#pragma CODE_SECTION(GetTemp2, ".TI.ramfunc");
#pragma CODE_SECTION(GetTemp3, ".TI.ramfunc");
#pragma CODE_SECTION(GetTemp4, ".TI.ramfunc");


float GetTemp1(void);

float GetTemp2(void);

float GetTemp3(void);

float GetTemp4(void);


uint16_t GetPotValue(void);

#pragma CODE_SECTION(shift, ".TI.ramfunc");
#pragma CODE_SECTION(CLR_Array, ".TI.ramfunc");
#pragma CODE_SECTION(Array_Set, ".TI.ramfunc");
#pragma CODE_SECTION(secfilt, ".TI.ramfunc");


void shift(float *array, int length);

void CLR_Array(float *array, int length);

void Array_Set(float *array, int length, float value);

/* 2nd Order Filter Section */
void secfilt(float *in, float *y, float *a,float *b, float g);



#endif
