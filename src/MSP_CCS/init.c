/*
 * init.c
 *
 *  Created on: Sep 21, 2016
 *      Author: Jay
 */

#include "msp430g2553.h"
#include "init.h"

void InitializeHardware(void)
{
	// set clock speed to 8 MHz
	DCOCTL = CALDCO_8MHZ;
	BCSCTL1 = CALBC1_8MHZ;

	// UART
	UCA0CTL1 |= UCSSEL_2 | UCSWRST;
	UCA0BR0 = 104;	// configure baud rate
	UCA0BR1 = 0;
	UCA0MCTL = UCBRS_1;
	UCA0STAT |= UCLISTEN;	// loop back mode
	UCA0CTL0 &= ~UCSWRST;

	// enable interrupts
	IE2 |= UCA0TXIE;
	IE2 |= UCA0RXIE;
}

void InitializeVariables(void)
{
}
