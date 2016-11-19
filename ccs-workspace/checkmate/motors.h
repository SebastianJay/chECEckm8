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

#include "driverlib.h"
#include "board_state.h"

/** Defines **/
#define SERVO_DELAY_BETWEEN_PULSE 	24000	// low ticks between pulses, 20 ms
// according to datasheet the full range is spanned by 0.75 ms (900 ticks) and 2.25 ms (2700 ticks) pulses
// in practice it seems we need to stretch these out a bit further
#define SERVO_PULSE_WIDTH_1			1800	// high ticks for engage, 1.5 ms
#define SERVO_PULSE_WIDTH_2			2700	// high ticks for disengage, 2.25 ms

#define ONE_SECOND_TICKS	12000000
#define SERVO_ENGAGE_DELAY	ONE_SECOND_TICKS
#define TABLE_MOVE_DELAY	ONE_SECOND_TICKS / 2

// these values are derived to apply 400 Hz square wave to STEP
#define X_STEP_TICKS 	15000		// ticks to wait in high or low for x stepping
#define Y_STEP_TICKS 	15000		// ticks to wait in high or low for y stepping
#define MOTOR_AWAKE_DELAY	48000	// ticks after awaking motor so it can "boot" (4 ms)

#define STEPS_PER_SPACE	308 	// steps needed to travel one board space

// ports and pins for motor control
#define X_STEP_PORT		GPIO_PORT_P2
#define X_STEP_PIN		GPIO_PIN5
#define X_DIR_PORT		GPIO_PORT_P3
#define X_DIR_PIN		GPIO_PIN0
#define X_SLEEP_PORT	GPIO_PORT_P5
#define X_SLEEP_PIN		GPIO_PIN7
#define Y_STEP_PORT		GPIO_PORT_P2
#define Y_STEP_PIN		GPIO_PIN7
#define Y_DIR_PORT		GPIO_PORT_P2
#define Y_DIR_PIN		GPIO_PIN6
#define Y_SLEEP_PORT	GPIO_PORT_P5
#define Y_SLEEP_PIN		GPIO_PIN6

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

// move table cursor to (0, 0), the bottom left corner
void goHome();
// move table cursor to (row, column), with option of using servo
void moveRC(int row, int column, int engage);

// move from one location to another, with option of using servo to magnetically drag piece
void move(piece_movement movement, int engage);
// helpers to accomplish move()
void stepX();
void stepY();
void moveX(int num_spaces);
void moveY(int num_spaces);

// DEBUG functions
// transitions between engaged and disengaged states forever
void debugServoLoop();
// does some simple motor movement
void debugMotorDemo();

#endif /* MOTORS_H_ */
