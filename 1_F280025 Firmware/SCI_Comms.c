#include "SCI_Comms.h"




/* GPIO initialization Rx&Tx coms*/
void InitRxTxPins(void){

  EALLOW;

  //GPIO28 Configurations (SCIA_Rx)
  GpioCtrlRegs.GPAGMUX2.bit.GPIO28 = 0;
  GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; // Pin MUXING: Configure GPIO28 as SCIa_Rx
  GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;  // Enable Pull-Up
  GpioCtrlRegs.GPADIR.bit.GPIO28 = 0;  //GPIO28 is input
  GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3; //Asynchronous input

  //GPIO29 Configurations (SCIA_Tx)
  GpioCtrlRegs.GPAGMUX2.bit.GPIO29 = 0;
  GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; // Pin MUXING: Configure GPIO28 as SCIa_Tx
  GpioCtrlRegs.GPAPUD.bit.GPIO28 = 1;  // Disable Pull-Up
  GpioCtrlRegs.GPADIR.bit.GPIO28 = 1;  //GPIO29 is input

  EDIS;
}

//  initSCIAEchoback - Initialize SCI-A for echoback
void initSCIAEchoback(void){

    SciaRegs.SCICCR.all = 0x0007;           // 1 stop bit,  No loopback
                                            // No parity, 8 char bits,
                                            // async mode, idle-line protocol
    SciaRegs.SCICTL1.all = 0x0003;          // enable TX, RX, internal SCICLK,
                                            // Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    //
    // SCIA at 9600 baud
    // @LSPCLK = 25 MHz (100 MHz SYSCLK) HBAUD = 0x01  and LBAUD = 0x44.
    //
    SciaRegs.SCIHBAUD.all = 0x0001;
    SciaRegs.SCILBAUD.all = 0x0044;


    SciaRegs.SCICTL1.all = 0x0023;          // Relinquish SCI from Reset
}

// initSCIAFIFO - Initialize the SCI FIFO
void initSCIAFIFO(void){

    SciaRegs.SCIFFTX.all = 0xE040;
    SciaRegs.SCIFFRX.all = 0x2041;
    SciaRegs.SCIFFCT.all = 0x0;

    SciaRegs.SCIFFRX.bit.RXFFIENA = 1;
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;
}

#pragma CODE_SECTION(transmitSCIAMessage, ".TI.ramfunc");
#pragma CODE_SECTION(transmitSCIAChar, ".TI.ramfunc");
#pragma CODE_SECTION(ftoa, ".TI.ramfunc");
#pragma CODE_SECTION(power, ".TI.ramfunc");
#pragma CODE_SECTION(reverse, ".TI.ramfunc");


// transmitSCIAChar - Transmit a character from the SCI
void transmitSCIAChar(uint16_t a){

    while (SciaRegs.SCIFFTX.bit.TXFFST != 0){

    }

    SciaRegs.SCITXBUF.all = a;
}


// transmitSCIAMessage - Transmit message via SCIA
void transmitSCIAMessage(char * msg){

    int i;
    i = 0;
    while(msg[i] != '\0'){

        transmitSCIAChar(msg[i]);
        i++;
    }
}



void reverse(char* str, int len){

    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}



int intToStr(int x, char str[], int d){

    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}



int LongIntToStr(uint32_t x, char str[], int d){

    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}


void ftoa(float n, char* res, int afterpoint){

    int i = 0;
    if (n<0.0f){
        res[0]='-';
        n=-n;
    }
    else{
        res[0]='\0';
    }
    int ipart = (int)n;
    float fpart = n - (float)ipart;
    if (ipart==0 && res[0]=='-'){
        res[1]='0';
        i=2;
    }
    else if (ipart==0){
        res[0]='0';
        i=1;
    }
    else {
        if (res[0]=='-'){
            i = intToStr(ipart, res+1, 0);
            i++;
        }
        else{
            i = intToStr(ipart, res, 0);
        }
    }
    if (afterpoint != 0) {
        res[i] = '.';
        fpart = fpart * power(10.f, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
    res[i+afterpoint+1]='\0';
}



float power(float x, int y){

    float temp;
    if( y == 0)
       return 1;
    temp = power(x, y/2);
    if (y%2 == 0)
        return temp*temp;
    else
    {
        if(y > 0)
            return x*temp*temp;
        else
            return (temp*temp)/x;
    }
}



float ReadFloatFromCommand(char * Command, char startChar, char endChar){
    char valueString[10];
    uint16_t tempIndex=0, tempIndexValue=0;
    float valueFloat=-1.f;


    /* Initially it searches for the start character */
    while (Command[tempIndex]!=startChar){
        tempIndex++;
    }

    tempIndex++; // Skips the start character

    while (Command[tempIndex]!=endChar){
        valueString[tempIndexValue]=Command[tempIndex];
        tempIndex++;
        tempIndexValue++;
    }
    valueString[tempIndexValue]='\0';

    //valueFloat = (float) atof(valueString);
    valueFloat = strtof(valueString,'\0');

    asm ("      NOP");

    return valueFloat;

}







