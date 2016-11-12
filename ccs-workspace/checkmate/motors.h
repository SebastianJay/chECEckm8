/*
 * motors.h
 *
 * Deals with interfacing with the XY table motors and servo
 *
 *  Created on: Oct 19, 2016
 *      Author: Jay
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include "board_state.h"

/** Defines **/
// corresponds to 20 ms
#define SERVO_DELAY_BETWEEN_PULSE 	24000	// low ticks
// according to datasheet the full range is spanned by 0.75 ms (900 ticks) and 2.25 ms (2700 ticks) pulses
// in practice it seems we need to stretch these out a bit further
#define SERVO_PULSE_WIDTH_1			625		// high ticks for servo state 1 (~520 us)
#define SERVO_PULSE_WIDTH_2			1700	// high ticks for servo state 2

/** Functions **/
// do any setup to send commands to motor driver
void initMotors();

// use the servo to move the magnet into position
void engageMagnet();
// use the servo to move the magnet out of position
void disengageMagnet();
// transitions between engaged and disengaged states forever
void debugServoLoop();

// move a piece from one location to another
void movePiece(piece_movement movement);


#endif /* MOTORS_H_ */
