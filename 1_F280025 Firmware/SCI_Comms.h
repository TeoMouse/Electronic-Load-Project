#include "f28x_project.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SCI_COMMS_H_
#define SCI_COMMS_H_

void InitRxTxPins(void);

//  initSCIAEchoback - Initialize SCI-A for echoback
void initSCIAEchoback(void);

// initSCIAFIFO - Initialize the SCI FIFO
void initSCIAFIFO(void);

#pragma CODE_SECTION(transmitSCIAMessage, ".TI.ramfunc");
#pragma CODE_SECTION(transmitSCIAChar, ".TI.ramfunc");
#pragma CODE_SECTION(ftoa, ".TI.ramfunc");
#pragma CODE_SECTION(power, ".TI.ramfunc");
#pragma CODE_SECTION(reverse, ".TI.ramfunc");


// transmitSCIAChar - Transmit a character from the SCI
void transmitSCIAChar(uint16_t a);



// transmitSCIAMessage - Transmit message via SCIA
void transmitSCIAMessage(char * msg);

void reverse(char* str, int len);


int intToStr(int x, char str[], int d);

int LongIntToStr(uint32_t x, char str[], int d);

void ftoa(float n, char* res, int afterpoint);

float power(float x, int y);

float ReadFloatFromCommand(char * Command, char startChar, char endChar);



#endif /* SCI_COMMS_H_ */
