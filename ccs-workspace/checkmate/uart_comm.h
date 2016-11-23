/*
 * uart_comm.h
 *
 * Deals with communication with remote machine.
 *
 *  Created on: Oct 19, 2016
 *      Author: Jay
 */

#ifndef UART_COMM_H_
#define UART_COMM_H_

#include "board_state.h"

/** Defines **/
#define UART_RECEIVE_BUFFER_LENGTH 32

#define UART_TX_PORT 	GPIO_PORT_P3
#define UART_TX_PIN		GPIO_PIN3
#define UART_RX_PORT	GPIO_PORT_P3
#define UART_RX_PIN		GPIO_PIN2
#define UART_EUSCI_BASE	EUSCI_A2_BASE
#define UART_EUSCI_INT	INT_EUSCIA2


/** Globals **/
volatile unsigned char gReceiveBuffer[UART_RECEIVE_BUFFER_LENGTH];
volatile unsigned short gReceiveBufferIndex;

/** Functions **/
// do any setup to send and receive through UART
void initUART();
// capture data from the UART receive register into global buffer
void uartReceiveISR();

// send a piece movement to chess server
void send(piece_movement* move);
// receive a piece movement response from chess server
// if other_move is filled, that move should be executed after the first move
// if a move has (0xFF, 0xFF) as a destination, then it represents a capture
signed char receive(piece_movement* move, piece_movement* other_move);

// DEBUG functions
void helloWorldSend();
void helloWorldReceive();
void debugGameLoop();

#endif /* UART_COMM_H_ */
