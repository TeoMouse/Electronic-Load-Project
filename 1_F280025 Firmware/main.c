#include "f28x_project.h"
#include "pwm.h"
#include "adc.h"
#include "control.h"
#include "math.h"
#include "SCI_Comms.h"

#define ILOAD_MAX       (30.f)
#define I_SET_MIN       (0)
#define I_SET_MAX       (30)
#define R_SET_MIN       (0.1f)
#define R_SET_MAX       (10000)
#define P_MAX           (320)

#define TEMP_MIN        (45) //Temperature value at which the fan is OFF
#define TEMP_MAX        (70) //Temperature value at which the fan is ON
#define TEMP_OFF_PROT	(110) //Temperature value at which everything is deactivated

#define CNT_OP          (1000)
#define CNT_TEMP_WARN   (250)

#define FAN_CNT_MAX     (10) //Increase fan duty every 10 interrupt cycles(1ms per cycle) 
#define FAN_D_STEP      (0.02f) // step of increament of the fan duty cycle

#define CNT_100US       (100)  // Timer 0 counter 100 us
#define CNT_1MS         (1000) // Timer 1 counter 1000 us
#define CNT_UART        (1000) //Count 1000 ms




/* -- LOW PASS FILTER with Fc @ ~100Hz definitions (Fs = 1kHz) -- */

float as1[3] = {1,   -1.8709,   0.8788};
float bs1[3] = {1,   -1.3027,  1};
float gain = 0.0114;

float ILoad_Refx[3] = {0, 0, 0},  ILoad_Refy[3] = {0, 0, 0};
float Temp1x[3] = {0, 0, 0},  Temp1y[3] = {0, 0, 0};
float Temp2x[3] = {0, 0, 0},  Temp2y[3] = {0, 0, 0};
float Temp3x[3] = {0, 0, 0},  Temp3y[3] = {0, 0, 0};
float Temp4x[3] = {0, 0, 0},  Temp4y[3] = {0, 0, 0};
float Itotal[3] = {0, 0, 0},  ItotalF[3] = {0, 0, 0};

/*------------------------------------------------------------------*/

/* -- LOW PASS FILTER with Fc @ ~1.5 kHz definitions (Fs = 10kHz) -- */

float af[3] = {1, -0.3679, 0}, bf[3] = {0, 0.6321, 0};
float Vin_x[3] = {0, 0, 0}, Vin_y[3] = {0, 0 ,0};

/*------------------------------------------------------------------*/


/* -- PI controller definitions (Fs = 10kHz) -- */


float kp = 0.02, ki = 40;
float Ts = 0.0001;

float dpi_C1[2] = {0,0}, epi_C1[2]={0,0}; //PI output -> dpi, PI input -> epi
float dpi_C2[2] = {0,0}, epi_C2[2]={0,0}; //PI output -> dpi, PI input -> epi
float dpi_C3[2] = {0,0}, epi_C3[2]={0,0}; //PI output -> dpi, PI input -> epi
float dpi_C4[2] = {0,0}, epi_C4[2]={0,0}; //PI output -> dpi, PI input -> epi
float dpi_C5[2] = {0,0}, epi_C5[2]={0,0}; //PI output -> dpi, PI input -> epi
float dpi_C6[2] = {0,0}, epi_C6[2]={0,0}; //PI output -> dpi, PI input -> epi


/*------------------------------------------------------------------*/



__interrupt void adcA1ISR(void);
__interrupt void adcC1ISR(void);
__interrupt void scia_rx_int(void);

void AckUsedInterrupts(void);

void main(void){

    //Initialize System Control:
    //PLL, WatchDog,enable Peripheral Clocks
    InitSysCtrl();

    
    //Initialize GPIO

    InitGpio();

    //Clear all __interrupts and initialize PIE vector table:
    //Disable CPU __interrupts  
    DINT;

    //Initialize the PIE control registers to their default state.
    InitPieCtrl();

    //Disable CPU interrupts and clear all CPU interrupt flags: 
    IER = 0x0000;
    IFR = 0x0000;


    //Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
    InitPieVectTable();

    // Enable PIE interrupt
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1; //ADCA interrupt
    PieCtrlRegs.PIEIER1.bit.INTx3 = 1; //ADCC interrupt
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1; //SCIA_RX interrupt


    EALLOW;
    PieVectTable.ADCA1_INT = &adcA1ISR;     // Function for ADCA interrupt
    PieVectTable.ADCC1_INT = &adcC1ISR;     // Function for ADCC interrupt
    PieVectTable.SCIA_RX_INT = &scia_rx_int; // Function for SCIA_RX interrupt
    EDIS;

    ConfigureADC();

    SetupADC();

    InitLEDGpio();

    InitRxTxPins();

    initSCIAEchoback();

    initSCIAFIFO();

    DELAY_US(1000);

    AckUsedInterrupts();

    DELAY_US(1000);

    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 100, CNT_100US); // Configure counter0 to count 100 us (Fsamp = 10 kHz)
    CpuTimer0Regs.TCR.all = 0x4000; //Clear timer0 flag

    ConfigCpuTimer(&CpuTimer1, 100, CNT_1MS); // Configure counter1 to count 1ms (Fsamp = 1 kHz)
    CpuTimer1Regs.TCR.all = 0x4000; //Clear timer1 flag

    // Enable global Interrupts and higher priority real-time debug events:
    IER |= M_INT1;  // Enable group 1 interrupts
    IER |= M_INT9;  // Enable group 9 interrupts


    EINT; // Enable Global interrupt INTM
    ERTM; // Enable Global realtime interrupt DBGM



    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    InitEPwm1Gpio();
    initEPWM1();

    InitEPwm2Gpio();
    initEPWM2();

    InitEPwm3Gpio();
    initEPWM3();

    InitEPwm5Gpio();
    initEPWM5();

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;



     while(1){

    }
}



uint16_t closed_loop = 1, mode = 0;

/* Mode: 0-> CC, 1-> CR, 2 -> CP*/

float I_c1, I_c2, I_c3, I_c4, I_c5, I_c6, Vin, Ic_Total;
float duty1 = 0.25, duty2 = 0.25, duty3 = 0.25, duty4 = 0.25, duty5 = 0.25, duty6 = 0.25, dutyFAN, Iload_Ref_Circ;
float duty = 0, duty_Max, dutyCNT;
float dpi_C1_Sat, dpi_C2_Sat, dpi_C3_Sat, dpi_C4_Sat, dpi_C5_Sat, dpi_C6_Sat;
uint16_t rst_c1, rst_c2, rst_c3, rst_c4, rst_c5, rst_c6, IntCNT;

__interrupt void adcA1ISR(void){

    /* Get input voltage value and filter it */

    Vin_x[0] = GetVin();
    secfilt(Vin_x , Vin_y, af, bf, 1);

    Vin = Vin_y[0];

     /* Shift the arrays for Vin filters */
     shift(Vin_x,3);
     shift(Vin_y,3);


    /* Get the current value for every circuit*/
    I_c1 = Get_Icir1();
    I_c2 = Get_Icir2();
    I_c3 = Get_Icir3();
    I_c4 = Get_Icir4();
    I_c5 = Get_Icir5();
    I_c6 = Get_Icir6();
    
    /* Select between closed loop and open loop operation
        closed_loop = 0: Open loop operation
        closed_loop = 1: Closed loop operation
    */
    switch(closed_loop){

        case 1:

            /* Calculate open loop duty acc. to I_LoadRef per circuit */
            duty_Max = 1.2f*DutyCalc(Iload_Ref_Circ);

            epi_C1[0] = Iload_Ref_Circ - I_c1;
            PIcontroller(dpi_C1, epi_C1, kp, ki, Ts); //PI output for circuit 1
            dpi_C1_Sat = saturator(0, duty_Max, dpi_C1[0]);

            /* Antiwind up error reset*/
            rst_c1 = AntiwindRST(dpi_C1[0], dpi_C1_Sat, epi_C1[0]);

            if (rst_c1 == 1){
                CLR_Array(epi_C1,2);
                Array_Set(dpi_C1,2,duty_Max);
            }


            Circuit1_Duty(dpi_C1_Sat); // Apply circuit 1 duty cycle

            epi_C2[0] = Iload_Ref_Circ - I_c2;
            PIcontroller(dpi_C2, epi_C2, kp, ki, Ts); //PI output for circuit 2
            dpi_C2_Sat = saturator(0, duty_Max, dpi_C2[0]);

            /* Antiwind up error reset*/
            rst_c2 = AntiwindRST(dpi_C2[0], dpi_C2_Sat, epi_C2[0]);

            if (rst_c2 == 1){
                CLR_Array(epi_C2,2);
                Array_Set(dpi_C2,2,duty_Max);
            }
            Circuit2_Duty(dpi_C2_Sat); // Apply circuit 2 duty cycle

            epi_C3[0] = Iload_Ref_Circ - I_c3;
            PIcontroller(dpi_C3, epi_C3, kp, ki, Ts); //PI output for circuit 3
            dpi_C3_Sat = saturator(0, duty_Max, dpi_C3[0]);

            /* Antiwind up error reset*/
            rst_c3 = AntiwindRST(dpi_C3[0], dpi_C3_Sat, epi_C3[0]);

            if (rst_c3== 1){
                CLR_Array(epi_C3,2);
                Array_Set(dpi_C3,2,duty_Max);
            }
            Circuit3_Duty(dpi_C3_Sat); // Apply circuit 3 duty cycle

            epi_C4[0] = Iload_Ref_Circ - I_c4;
            PIcontroller(dpi_C4, epi_C4, kp, ki, Ts); //PI output for circuit 4
            dpi_C4_Sat = saturator(0, duty_Max, dpi_C4[0]);

            /* Antiwind up error reset*/
            rst_c4 = AntiwindRST(dpi_C4[0], dpi_C4_Sat, epi_C4[0]);

            if (rst_c4 == 1){
                CLR_Array(epi_C4,2);
                Array_Set(dpi_C4,2,duty_Max);
            }
            Circuit4_Duty(dpi_C4_Sat); // Apply circuit 4 duty cycle

            epi_C5[0] = Iload_Ref_Circ - I_c5;
            PIcontroller(dpi_C5, epi_C5, kp, ki, Ts); //PI output for circuit 5
            dpi_C5_Sat = saturator(0, duty_Max, dpi_C5[0]);

            /* Antiwind up error reset*/
            rst_c5 = AntiwindRST(dpi_C5[0], dpi_C5_Sat, epi_C5[0]);

            if (rst_c5 == 1){
                CLR_Array(epi_C5,2);
                Array_Set(dpi_C5,2,duty_Max);
            }
            Circuit5_Duty(dpi_C5_Sat); // Apply circuit 5 duty cycle

            epi_C6[0] = Iload_Ref_Circ - I_c6;
            PIcontroller(dpi_C6, epi_C6, kp, ki, Ts); //PI output for circuit 6
            dpi_C6_Sat = saturator(0, duty_Max, dpi_C6[0]);

            /* Antiwind up error reset*/
            rst_c6 = AntiwindRST(dpi_C6[0], dpi_C6_Sat, epi_C6[0]);

            if (rst_c6 == 1){
                CLR_Array(epi_C6,2);
                Array_Set(dpi_C6,2,duty_Max);
            }
            Circuit6_Duty(dpi_C6_Sat); // Apply circuit 6 duty cycle

            /* Shift array values for the PI controllers */
            shift(dpi_C1,2);
            shift(epi_C1,2);
            shift(dpi_C2,2);
            shift(epi_C2,2);
            shift(dpi_C3,2);
            shift(epi_C3,2);
            shift(dpi_C4,2);
            shift(epi_C4,2);
            shift(dpi_C5,2);
            shift(epi_C5,2);
            shift(dpi_C6,2);
            shift(epi_C6,2);


            break;

        case 0:

            /* Calculate open loop duty acc. to I_LoadRef per circuit */   
            duty = DutyCalc(Iload_Ref_Circ);

            /* Apply the calculated duty cycle per circuit*/ 
            Circuit1_Duty(duty);
            Circuit2_Duty(duty);
            Circuit3_Duty(duty);
            Circuit4_Duty(duty);
            Circuit5_Duty(duty);
            Circuit6_Duty(duty);

            /* Clear PI controller arrays */
            CLR_Array(dpi_C1,2);
            CLR_Array(epi_C1,2);
            CLR_Array(dpi_C2,2);
            CLR_Array(epi_C2,2);
            CLR_Array(dpi_C3,2);
            CLR_Array(epi_C3,2);
            CLR_Array(dpi_C4,2);
            CLR_Array(epi_C4,2);
            CLR_Array(dpi_C5,2);
            CLR_Array(epi_C5,2);
            CLR_Array(dpi_C6,2);
            CLR_Array(epi_C6,2);

            break;
    }




    // Acknowledge the interrupt
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

uint16_t PotValue, FanEn, cnt, LED_St, CNT_St, cnt_comms, OVT, OVT_Clear, ComRec;
float  ILoad_Ref, I_user, R_user = 100, Pcalc, TempMax, ComFloat, Ptarget, P_user;
uint16_t SCIATXindex,SCIA_Arr_Ind, ManFault;

char RecCommand[25]="\0";
char  MonMSG[100];

__interrupt void adcC1ISR(void){

    char TempStr[25], firstWord[25];
    uint16_t tempIndex = 0;

    /* Get potentiometer ADC value */
    PotValue = GetPotValue();

    Itotal[0] = I_c1 + I_c2 + I_c3 + I_c4 + I_c5 + I_c6;

    /* Get temperature values */ 
    Temp1x[0] = GetTemp1();
    Temp2x[0] = GetTemp2();
    Temp3x[0] = GetTemp3();
    Temp4x[0] = GetTemp4();

    /* Filter temperature values */ 
    secfilt(Temp1x , Temp1y, as1, bs1, gain);
    secfilt(Temp2x , Temp2y, as1, bs1, gain);
    secfilt(Temp3x , Temp3y, as1, bs1, gain);
    secfilt(Temp4x , Temp4y, as1, bs1, gain);
    secfilt(Itotal , ItotalF, as1, bs1, gain);


    /* Shift the arrays for temperature filters */ 
    shift(Temp1x,3);
    shift(Temp1y,3);
    shift(Temp2x,3);
    shift(Temp2y,3);
    shift(Temp3x,3);
    shift(Temp3y,3);
    shift(Temp4x,3);
    shift(Temp4y,3);
    shift(Itotal,3);
    shift(ItotalF,3);


    Pcalc = Vin* ItotalF[0]; // Calculate electronic load power

    switch(mode){
        case 0: //CC mode
            ILoad_Refx[0]  = saturator(I_SET_MIN, I_SET_MAX, I_user);
            Ptarget = Vin*ILoad_Refx[0];

            if (Ptarget< (P_MAX - 10) ){
                ILoad_Refx[0]  = saturator(I_SET_MIN, I_SET_MAX, I_user);
            }else if (Ptarget > P_MAX){
                ILoad_Refx[0] = P_MAX/Vin;
            }   

        break;

        case 1: // CR Mode
            R_user = saturator(R_SET_MIN, R_SET_MAX, R_user);
            ILoad_Refx[0] = Vin/R_user;//PotValue*ILOAD_MAX/4095.f; //Set the maximum Load current acc. to potent. value
            ILoad_Refx[0] = saturator(I_SET_MIN, I_SET_MAX, ILoad_Refx[0]);
            Ptarget = Vin*ILoad_Refx[0];

            if (Ptarget< (P_MAX - 10) ){
                ILoad_Refx[0] = Vin/R_user;//PotValue*ILOAD_MAX/4095.f; //Set the maximum Load current acc. to potent. value
                ILoad_Refx[0] = saturator(I_SET_MIN, I_SET_MAX, ILoad_Refx[0]);
            }else if (Ptarget > P_MAX){
                ILoad_Refx[0] = P_MAX/Vin;

            }
           
        break;

        case 2: // CP Mode

            ILoad_Refx[0] = P_user/Vin;
            Ptarget = Vin*ILoad_Refx[0];

            if (Ptarget< (P_MAX - 10) ){
                ILoad_Refx[0] = P_user/Vin;
                ILoad_Refx[0] = saturator(I_SET_MIN, I_SET_MAX, ILoad_Refx[0]);
            }
            else if (Ptarget > P_MAX){
                ILoad_Refx[0] = P_MAX/Vin;
            }

        break;

    }


    //ILoad_Refx[0] = I_user;//PotValue*ILOAD_MAX/4095.f; //Set the maximum Load current acc. to potent. value

    /* Filter ILoad reference value */
    secfilt(ILoad_Refx , ILoad_Refy, as1, bs1, gain);
    shift(ILoad_Refx,3);
    shift(ILoad_Refy,3);

    Iload_Ref_Circ = ILoad_Refy[0]/6.f; // Extract Reference value per circuit

    
    if (Temp1y[0]>TEMP_MAX || Temp2y[0]>TEMP_MAX || Temp3y[0]>TEMP_MAX || Temp4y[0]>TEMP_MAX || FanEn == 1){

    	IntCNT++;
    	if (IntCNT > FAN_CNT_MAX-1) IntCNT = 0; // Allow the increment of the Fan Duty every FAN_CNT_MAX cylces of this interrupt

        if (dutyCNT< (1+FAN_D_STEP) && IntCNT == 0)  dutyCNT = dutyCNT + FAN_D_STEP;

        dutyCNT = saturator(0, 1, dutyCNT);

        FanDuty(dutyCNT);//FanDuty(0.0267f*ILoad_Refy[0]+0.3f); // Adjust fan duty cycle acc. to I_Load
        CNT_St = 1;

    }
    else if (Temp1y[0]<TEMP_MIN && Temp2y[0]<TEMP_MIN && Temp3y[0]<TEMP_MIN && Temp4y[0]<TEMP_MIN && FanEn ==0){
        dutyCNT = 0;
        FanDuty(dutyCNT);
        CNT_St = 0;
    }

    /* User LED control */
    cnt++;
    if (cnt > (CNT_St*CNT_TEMP_WARN + (1-CNT_St)*CNT_OP)){
        cnt = 0;
        if(LED_St == 0){
            LED2_ON();
            LED_St = 1;
        }
        else{
            LED2_OFF();
            LED_St = 0;
        }
    }
    if (OVT == 1) LED2_ON();

    if ((Temp1y[0]>TEMP_OFF_PROT || Temp2y[0]>TEMP_OFF_PROT || Temp3y[0]>TEMP_OFF_PROT || Temp4y[0]>TEMP_OFF_PROT || ManFault == 1) && OVT == 0){
    	EALLOW;
    	EPwm1Regs.TZFRC.bit.OST = 1; 
        EPwm2Regs.TZFRC.bit.OST = 1; 
        EPwm3Regs.TZFRC.bit.OST = 1; 
        EPwm5Regs.TZFRC.bit.OST = 1; 
        EDIS;

        OVT = 1;
        I_user = 0;
        R_user = 100;
        P_user = 0;
        ManFault = 0;

    }
    if (OVT_Clear == 1){

    	EALLOW;
        EPwm1Regs.TZCLR.bit.OST = 1; 
        EPwm2Regs.TZCLR.bit.OST = 1; 
        EPwm3Regs.TZCLR.bit.OST = 1; 
        EPwm5Regs.TZCLR.bit.OST = 1; 
        EDIS;

        OVT = 0;
        OVT_Clear = 0;

        /* Clear PI controller arrays */
        CLR_Array(dpi_C1,2);
        CLR_Array(epi_C1,2);
        CLR_Array(dpi_C2,2);
        CLR_Array(epi_C2,2);
        CLR_Array(dpi_C3,2);
        CLR_Array(epi_C3,2);
        CLR_Array(dpi_C4,2);
        CLR_Array(epi_C4,2);
        CLR_Array(dpi_C5,2);
        CLR_Array(epi_C5,2);
        CLR_Array(dpi_C6,2);
        CLR_Array(epi_C6,2);
    }

    /* Communication every CNT_COMMS cycles x 1ms */
    cnt_comms++;

    if (cnt_comms> CNT_UART){

        cnt_comms = 0;

        /* Calculate the maximum thermocouple value to send it via SCIA */
        TempMax = 0;
        if (Temp1y[0]>TempMax) TempMax = Temp1y[0];
        if (Temp2y[0]>TempMax) TempMax = Temp2y[0];
        if (Temp3y[0]>TempMax) TempMax = Temp3y[0];
        if (Temp4y[0]>TempMax) TempMax = Temp4y[0];
        

        MonMSG[0] = '\0';
        
        /* Transmit monitoring message */

        strcat(MonMSG,"\r\nV_Load = ");
        ftoa(Vin,TempStr,2);
        strcat(MonMSG,TempStr);
        strcat(MonMSG,", "); // Send the load voltage

        strcat(MonMSG,"I_Load = ");
        ftoa(ItotalF[0],TempStr,2);
        strcat(MonMSG,TempStr);
        strcat(MonMSG,", "); // Send the load current

        strcat(MonMSG,"P_Load = ");
        ftoa(Pcalc,TempStr,2);
        strcat(MonMSG,TempStr);
        strcat(MonMSG,", "); // Send the power of the load

        strcat(MonMSG,"Max_Temp = ");
        ftoa(TempMax,TempStr,2);
        strcat(MonMSG,TempStr);
        strcat(MonMSG,", "); // Send the MaxTemp of the thermocouples

        if (mode == 0){
            strcat(MonMSG,"Mode_CC");
            strcat(MonMSG,", "); // Send the mode of operation

            strcat(MonMSG,"Ref_");
            ftoa(I_user,TempStr,2);
            strcat(MonMSG,TempStr);
            strcat(MonMSG,'\0'); // Send the reference value of the mode
        }
        else if (mode == 1){
            strcat(MonMSG,"Mode_CR");
            strcat(MonMSG,", "); // Send the mode of operation

            strcat(MonMSG,"Ref_");
            ftoa(R_user,TempStr,2);
            strcat(MonMSG,TempStr);
            strcat(MonMSG,'\0'); // Send the reference value of the mode
        }
        else{
            strcat(MonMSG,"Mode_CP");
            strcat(MonMSG,", "); // Send the mode of operation

            strcat(MonMSG,"Ref_");
            ftoa(P_user,TempStr,2);
            strcat(MonMSG,TempStr);
            strcat(MonMSG,"\0"); // Send the reference value of the mode
        }


        transmitSCIAMessage(MonMSG); // Transmit the monitoring data via SCIA

        transmitSCIAMessage("\r\n");

        if (ComRec == 1){ /* if command is received */

            /* Decode the received command */

            /* Extract the first word of the command */
            while ((RecCommand[tempIndex]!=';') && ((RecCommand[tempIndex]!=' '))){
                firstWord[tempIndex]=RecCommand[tempIndex];
                tempIndex++;
            }
            firstWord[tempIndex]=';';

            if (strncmp(firstWord,"&current;",strlen("&current;"))==0){
                I_user = ReadFloatFromCommand(RecCommand,' ',';');
            }
            else if (strncmp(firstWord,"&resistor;",strlen("&resistor;"))==0){
                R_user = ReadFloatFromCommand(RecCommand,' ',';');
            }
            else if (strncmp(firstWord,"&power;",strlen("&power;"))==0){
                P_user = ReadFloatFromCommand(RecCommand,' ',';');
            }
            else if (strncmp(firstWord,"&ModeI;",strlen("&ModeI;"))==0){
                mode = 0; // CC Mode
                transmitSCIAMessage("Constant Current mode is active.\r\n");

            }else if (strncmp(firstWord,"&ModeR;",strlen("&ModeR;"))==0){
                mode = 1; // CR Mode
                transmitSCIAMessage("Constant Resistance mode is active.\r\n");
            }
            else if (strncmp(firstWord,"&ModeP;",strlen("&ModeP;"))==0){
                mode = 2; // CP Mode
                transmitSCIAMessage("Constant Power mode is active.\r\n");
            }
            else if (strncmp(firstWord,"&FanEN;",strlen("&FanEN;"))==0){
                FanEn = 1;
                transmitSCIAMessage("Fan is enabled.\r\n");
            }
            else if (strncmp(firstWord,"&FanDIS;",strlen("&FanDIS;"))==0){
                FanEn = 0;
                transmitSCIAMessage("Fan is automatically controlled.\r\n");
            }
            else if (strncmp(firstWord,"&GenFault;",strlen("&GenFault;"))==0){
                transmitSCIAMessage("Fault in manually generated.\r\n");
                ManFault = 1;
            }
            else if (strncmp(firstWord,"&CLRFault;",strlen("&CLRFault;"))==0){
                OVT_Clear = 1;
                transmitSCIAMessage("Fault is cleared.\r\n");
            }
            else{
                transmitSCIAMessage("Invalid Command !!!\r\n");
            }

            SCIATXindex = 1;
            ComRec = 0;


            for (SCIA_Arr_Ind = 0;SCIA_Arr_Ind < 24;SCIA_Arr_Ind++) RecCommand[SCIA_Arr_Ind] = '\0';

        }

    }

    // Acknowledge the interrupt
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}



/* SCIA Interrupt */
__interrupt void scia_rx_int(void){
    char receivedChar;
    receivedChar = SciaRegs.SCIRXBUF.all;

    if (receivedChar=='&') {
        SCIATXindex=1;
    }

    if (SCIATXindex < 25){
        RecCommand[SCIATXindex-1]=receivedChar;
    }

    SCIATXindex++;

    if (receivedChar==';'){        

        receivedChar='\0';
        ComRec = 1; /* Command is received */
    }

    if (SCIATXindex > 25) SCIATXindex=1;

    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; // clear flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9; //Acknowledge Interrupt from Group9


}

void AckUsedInterrupts(void){

    // Acknowledge the interrupt
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; // clear flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9; //Acknowledge Interrupt from Group9

}

