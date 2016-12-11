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

	// set input button pins
	MAP_GPIO_setAsInputPin(X_HOMING_BUTTON_PORT, X_HOMING_BUTTON_PIN);
	MAP_GPIO_setAsInputPin(Y_HOMING_BUTTON_PORT, Y_HOMING_BUTTON_PIN);

	// init globals
	gTableCursor.r = 0;
	gTableCursor.c = 0;
}

void debugMotorDemo() {
//	disengageMagnet();

//	moveToHome();
	while (1) {
		moveRC(2, 2, FALSE);
		moveRC(0, 0, FALSE);
	}
//	moveToHome();
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

void debugButtonDemo()
{
	// set output LED
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
			GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

	while (1) {
		_delay_cycles(100);
		int val = MAP_GPIO_getInputPinValue(X_HOMING_BUTTON_PORT, X_HOMING_BUTTON_PIN);
		if (val == GPIO_INPUT_PIN_LOW)
		{
			break;
		}
	}

	// light LED when button pressed
	MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void stepX() {
	MAP_GPIO_setOutputHighOnPin(X_STEP_PORT, X_STEP_PIN);
	_delay_cycles(X_STEP_TICKS);
	MAP_GPIO_setOutputLowOnPin(X_STEP_PORT, X_STEP_PIN);
	_delay_cycles(X_STEP_TICKS);
}

void moveX(int num_spaces) {
	// Bounds check
	if (num_spaces == 0 || num_spaces <= -8 || num_spaces >= 8) {
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
	_delay_cycles(MOTOR_MOVE_DELAY);
}

void stepY() {
	MAP_GPIO_setOutputHighOnPin(Y_STEP_PORT, Y_STEP_PIN);
	_delay_cycles(Y_STEP_TICKS);
	MAP_GPIO_setOutputLowOnPin(Y_STEP_PORT, Y_STEP_PIN);
	_delay_cycles(Y_STEP_TICKS);
}

void moveY(int num_spaces) {
	// Bounds check
	if (num_spaces == 0 || num_spaces <= -8 || num_spaces >= 8) {
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
//	MAP_GPIO_setOutputLowOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction down

	int i;
	int j;
	for (i = 0; i < num_spaces; i++) {
		for (j = 0; j < STEPS_PER_SPACE; j++) {
			stepY();
		}
	}
	MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_MOVE_DELAY);
}

void moveBetweenCornerAndCenter(int toCorner) {
	MAP_GPIO_setOutputHighOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	MAP_GPIO_setOutputHighOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_AWAKE_DELAY);
	if (toCorner) {
		MAP_GPIO_setOutputHighOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction left
		MAP_GPIO_setOutputLowOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction down
	} else {
		MAP_GPIO_setOutputLowOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction right
		MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction up
	}
	int j;
	for (j = 0; j < STEPS_PER_HALF_SPACE; j++) {
		// interleave steps to both motors
		stepX();
		stepY();
	}
	MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_MOVE_DELAY);
}

void moveToButtons() {

	MAP_GPIO_setOutputHighOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction left
	MAP_GPIO_setOutputLowOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction down

	// Delay and wake
	_delay_cycles(MOTOR_AWAKE_DELAY);
	MAP_GPIO_setOutputHighOnPin(X_SLEEP_PORT, X_SLEEP_PIN);

	// Home on X
	while (1) {
		int val = MAP_GPIO_getInputPinValue(X_HOMING_BUTTON_PORT, X_HOMING_BUTTON_PIN);
		if (val == GPIO_INPUT_PIN_LOW)
		{
			break;
		}
		stepX();
	}

	_delay_cycles(MOTOR_MOVE_DELAY);

	// Home on Y
	while (1) {
		int val = MAP_GPIO_getInputPinValue(Y_HOMING_BUTTON_PORT, Y_HOMING_BUTTON_PIN);
		if (val == GPIO_INPUT_PIN_LOW)
		{
			break;
		}
		stepY();
	}

	MAP_GPIO_setOutputLowOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction right
	MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction up

	// Move to column A (x-axis)
	int i;
	for (i = 0; i < STEPS_COLUMN_A; i++)
		stepX();
	}

	// Move to row 1 (y-axis)
	for (i = 0; i < STEPS_ROW_ONE; i++)
		stepY();
	}

	// Sleep and delay
	MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	_delay_cycles(MOTOR_MOVE_DELAY);
}

void moveToHome() {
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
		// pull magnet up
		engageMagnet();
		// move to corner of square (to avoid collisions)
		moveBetweenCornerAndCenter(TRUE);
	}

	if (movement.rEnd == -1 && movement.cEnd == -1)
	{
		// TODO move piece to capture space
		return;
	}

	// Compute position difference
	x_move = movement.cEnd - movement.cStart;
	y_move = movement.rEnd - movement.rStart;

	// move the piece to the destination
	moveX(x_move);
	moveY(y_move);

	if (engage) {
		// move to center of square
		moveBetweenCornerAndCenter(FALSE);
		// pull magnet down
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
