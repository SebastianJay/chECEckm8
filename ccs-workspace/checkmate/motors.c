/*
 * motors.c
 *
 *  Created on: Nov 6, 2016
 *      Author: Jay
 */

#include "driverlib.h"
#include "board_state.h"
#include "motors.h"

/*
 * Timer_A PWM Configuration Parameter
 * PWM is used to control the servo; initial config has magnet on servo in disengaged state
 */
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10,
		SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_2,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
		SERVO_PULSE_WIDTH_2
};

void initMotors()
{
	// clock source for SMCLK
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // choose GPIO pin for PWM
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    // Configure Timer A PWM
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    // Configure motor driver pins
	// X axis STEP
	MAP_GPIO_setAsOutputPin(X_STEP_PORT, X_STEP_PIN);
	MAP_GPIO_setOutputLowOnPin(X_STEP_PORT, X_STEP_PIN);

	// X axis DIR: LOW -> RIGHT, HIGH -> LEFT
	MAP_GPIO_setAsOutputPin(X_DIR_PORT, X_DIR_PIN);
	MAP_GPIO_setOutputLowOnPin(X_DIR_PORT, X_DIR_PIN);

	// X axis SLEEP
	MAP_GPIO_setAsOutputPin(X_SLEEP_PORT, X_SLEEP_PIN);
	MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);

	// Y axis STEP
	MAP_GPIO_setAsOutputPin(Y_STEP_PORT, Y_STEP_PIN);
	MAP_GPIO_setOutputLowOnPin(Y_STEP_PORT, Y_STEP_PIN);

	// Y axis DIR: LOW -> DOWN, HIGH -> UP
	MAP_GPIO_setAsOutputPin(Y_DIR_PORT, Y_DIR_PIN);
	MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);

	// Y axis SLEEP
	MAP_GPIO_setAsOutputPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);

	// init globals
	gTableCursor.r = 0;
	gTableCursor.c = 0;
}

void debugMotorDemo() {
	disengageMagnet();

	goHome();
	moveRC(1, 2, TRUE);
	moveRC(0, 0, TRUE);
}

void debugServoLoop()
{
	while(1) {
		int i;
		engageMagnet();
		for (i = 0; i < 1000000; i++);
		disengageMagnet();
		for (i = 0; i < 1000000; i++);
	}
}

void stepX() {
	MAP_GPIO_setOutputHighOnPin(X_STEP_PORT, X_STEP_PIN);
	_delay_cycles(X_STEP_TICKS);
	MAP_GPIO_setOutputLowOnPin(X_STEP_PORT, X_STEP_PIN);
	_delay_cycles(X_STEP_TICKS);
}

void moveX(int num_spaces) {
	if (num_spaces == 0) {
		return;
	}
	MAP_GPIO_setOutputHighOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	_delay_cycles(MOTOR_AWAKE_DELAY);
	if (num_spaces < 0) {
		num_spaces = -num_spaces;
		MAP_GPIO_setOutputHighOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction left
	} else {
		MAP_GPIO_setOutputLowOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction right
	}
	int i;
	int j;
	for (i = 0; i < num_spaces; i++) {
		for (j = 0; j < STEPS_PER_SPACE; j++) {
			stepX();
		}
	}
	MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	_delay_cycles(TABLE_MOVE_DELAY);
}

void stepY() {
	MAP_GPIO_setOutputHighOnPin(Y_STEP_PORT, Y_STEP_PIN);
	_delay_cycles(Y_STEP_TICKS);
	MAP_GPIO_setOutputLowOnPin(Y_STEP_PORT, Y_STEP_PIN);
	_delay_cycles(Y_STEP_TICKS);
}

void moveY(int num_spaces) {
	if (num_spaces == 0) {
		return;
	}
	MAP_GPIO_setOutputHighOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_AWAKE_DELAY);
	if (num_spaces < 0) {
		num_spaces = -num_spaces;
		MAP_GPIO_setOutputLowOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction down
	} else {
		MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction up
	}
	int i;
	int j;
	for (i = 0; i < num_spaces; i++) {
		for (j = 0; j < STEPS_PER_SPACE; j++) {
			stepY();
		}
	}
	MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(TABLE_MOVE_DELAY);
}

void goHome() {
	moveRC(0, 0, FALSE);
}

void moveRC(int row, int column, int engage) {
	piece_movement movement;
	movement.cStart = gTableCursor.c;
	movement.cEnd = column;
	movement.rStart = gTableCursor.r;
	movement.rEnd = row;

	move(movement, engage);
}

void move(piece_movement movement, int engage) {
	// Compute difference from cursor to start location
	int x_move;
	int y_move;
	x_move = movement.cStart - gTableCursor.c;
	y_move = movement.rStart - gTableCursor.r;

	// move to the piece's source
	moveX(x_move);
	moveY(y_move);

	if (engage) {
		engageMagnet();
	}

	if (movement.rEnd == -1 && movement.cEnd == -1)
	{
		// TODO move piece to capture space
		return;
	}

	// Compute position difference
	x_move = movement.cEnd - movement.cStart;
	y_move = movement.rEnd - movement.rStart;

	// TODO move to corner of square

	// move the piece to the destination
	moveX(x_move);
	moveY(y_move);

	// TODO move to center of square

	if (engage) {
		disengageMagnet();
	}

	// set cursor to destination
	gTableCursor.r = movement.rEnd;
	gTableCursor.c = movement.cEnd;
}

void engageMagnet()
{
	pwmConfig.timerPeriod = SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_1;
	pwmConfig.dutyCycle = SERVO_PULSE_WIDTH_1;
	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
	_delay_cycles(SERVO_ENGAGE_DELAY);
}

void disengageMagnet()
{
	pwmConfig.timerPeriod = SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_2;
	pwmConfig.dutyCycle = SERVO_PULSE_WIDTH_2;
	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
	_delay_cycles(SERVO_ENGAGE_DELAY);
}
