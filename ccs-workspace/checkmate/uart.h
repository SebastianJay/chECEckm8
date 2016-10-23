/*
 * uart.h
 *
 * Deals with communication with remote machine.
 *
 *  Created on: Oct 19, 2016
 *      Author: Jay
 */

#ifndef UART_H_
#define UART_H_

#include "board_state.h"

/** Functions **/
// do any setup to send and receive through UART
void initUART();

// send a piece movement to chess server
void send(piece_movement move);
// receive a piece movement response from chess server
void receive(piece_movement* move);

#endif /* UART_H_ */
