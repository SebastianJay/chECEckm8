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

#define X_STEP_ON 500	// amount of high ticks for stepping once in x direction
#define X_STEP_OFF 2500	// amount of low ticks for x stepping
#define Y_STEP_ON 500	// amount of high ticks for stepping once in y direction
#define Y_STEP_OFF 2500	// amount of low ticks for y stepping
#define STEPS_PER_SPACE	308 	// number of steps needed to travel one board space

/** Structs **/
typedef struct {
	signed char r;
	signed char c;
} tableCursor;

/** Globals **/
tableCursor gTableCursor;

/** Functions **/
// do any setup to send commands to motor driver
void initMotors();

// use the servo to move the magnet into position
void engageMagnet();
// use the servo to move the magnet out of position
void disengageMagnet();

// move a piece from one location to another
void movePiece(piece_movement movement);
void step_x();
void move_x(int num_spaces);
void step_y();
void move_y(int num_spaces);

// DEBUG functions
// transitions between engaged and disengaged states forever
void debugServoLoop();
void debugMotorDemo();

#endif /* MOTORS_H_ */
