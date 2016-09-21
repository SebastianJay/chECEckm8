/*
 * uart.h
 *
 *  Created on: Sep 21, 2016
 *      Author: Jay
 */

#ifndef UART_H_
#define UART_H_

// macros
#define BUFFER_SIZE 128

// global vars
unsigned char gTransmitBuffer[BUFFER_SIZE];
unsigned char gReceiveBuffer[BUFFER_SIZE];
unsigned int gTransmitBufferIndex;
unsigned int gReceiveBufferIndex;

// functions
void Update();
int Transmit();
int Receive();

__interrupt void TxInterrupt(void);
__interrupt void RxInterrupt(void);

#endif /* UART_H_ */
