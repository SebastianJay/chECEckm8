/*
 * main.c
 *
 *  Created on: Oct 23, 2016
 *      Author: Jay
 */

#include "driverlib.h"
#include "msp.h"

#include "uart_comm.h"
#include "motors.h"

void main()
{
	// Stop watchdog timer
    MAP_WDT_A_holdTimer();

	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
	MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	/** Servo debugging **/
	//initMotors();
	//debugServoLoop();

	/** UART debugging **/
	initUART();
	//helloWorldSend();
	//helloWorldReceive();
	debugGameLoop();

	//MAP_Interrupt_enableMaster();
    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
