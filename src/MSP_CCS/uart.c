/*
 * uart.c
 *
 *  Created on: Sep 21, 2016
 *      Author: Jay
 */

#include "uart.h"
#include "msp430g2553.h"

void Update()
{

}

int Transmit()
{
	return 0;
}

int Receive()
{
	return 0;
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void TxInterrupt(void)
{
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void RxInterrupt(void)
{
	IFG2 &= ~UCA0RXIFG;
}
