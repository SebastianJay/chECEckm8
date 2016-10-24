/*
 * main.c
 *
 *  Created on: Oct 23, 2016
 *      Author: Jay
 */

#include "driverlib.h"
#include "msp.h"

#include "uart_comm.h"

void main()
{
	WDTCTL = WDTPW | WDTHOLD;                    /* Stop watchdog timer */

	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
	MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	initUART();
	//helloWorldSend();
	helloWorldReceive();

	MAP_Interrupt_enableMaster();
    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
