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
	MAP_GPIO_setOutputHighOnPin(X_DIR_PORT, X_DIR_PIN);

	// X axis SLEEP
	MAP_GPIO_setAsOutputPin(X_SLEEP_PORT, X_SLEEP_PIN);
	MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);

	// Y axis STEP
	MAP_GPIO_setAsOutputPin(Y_STEP_PORT, Y_STEP_PIN);
	MAP_GPIO_setOutputLowOnPin(Y_STEP_PORT, Y_STEP_PIN);

	// Y axis DIR: LOW -> UP, HIGH -> DOWN
	MAP_GPIO_setAsOutputPin(Y_DIR_PORT, Y_DIR_PIN);
	MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);

	// Y axis SLEEP
	MAP_GPIO_setAsOutputPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);

	// set input button pins
	MAP_GPIO_setOutputHighOnPin(X_HOMING_BUTTON_PORT, X_HOMING_BUTTON_PIN);
	MAP_GPIO_setOutputHighOnPin(Y_HOMING_BUTTON_PORT, Y_HOMING_BUTTON_PIN);
	MAP_GPIO_setAsInputPin(X_HOMING_BUTTON_PORT, X_HOMING_BUTTON_PIN);
	MAP_GPIO_setAsInputPin(Y_HOMING_BUTTON_PORT, Y_HOMING_BUTTON_PIN);

	// init globals
	gTableCursor.r = 0;
	gTableCursor.c = 0;
	gCaptureIndex = 0;
}

void debugMotorDemo() {
	moveRC(0, 0, FALSE);
	moveRC(2, 2, TRUE);

	moveRC(1, 4, FALSE);
	moveRC(3, 4, TRUE);

	moveRC(1, 3, FALSE);
	moveRC(2, 3, TRUE);

	moveRC(1, 6, FALSE);
	moveRC(2, 5, TRUE);
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
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

	while (1) {
		_delay_cycles(100);
		int val = MAP_GPIO_getInputPinValue(Y_HOMING_BUTTON_PORT, Y_HOMING_BUTTON_PIN);
		if (val == GPIO_INPUT_PIN_LOW)
		{
			volatile int x;
			break;
		}
	}

	// light LED when button pressed
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
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
		MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction down
	} else {
		MAP_GPIO_setOutputLowOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction up
	}

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

void moveXY(int x_num_spaces, int y_num_spaces) {
	if (x_num_spaces <= -8 || x_num_spaces >= 8 || y_num_spaces <= -8 || y_num_spaces >= 8) {
		return;
	}
	if (ABSVAL(x_num_spaces) != ABSVAL(y_num_spaces)) {
		return;	// only handle equal number of tile movements each way
	}
	int num_spaces = ABSVAL(x_num_spaces);

	MAP_GPIO_setOutputHighOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	MAP_GPIO_setOutputHighOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_AWAKE_DELAY);
	if (x_num_spaces < 0) {
		MAP_GPIO_setOutputHighOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction left
	} else {
		MAP_GPIO_setOutputLowOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction right
	}
	if (y_num_spaces < 0) {
		MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction down
	} else {
		MAP_GPIO_setOutputLowOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction up
	}

	int i;
	int j;
	for (i = 0; i < num_spaces; i++) {
		for (j = 0; j < STEPS_PER_SPACE; j++) {
			stepX();
			stepY();
		}
	}
	MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_MOVE_DELAY);
}

void moveHalfTile(int xdir, int ydir)
{
	if (ydir != 0)
	{
		MAP_GPIO_setOutputHighOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
		_delay_cycles(MOTOR_AWAKE_DELAY);
		if (ydir > 0)
		{
			MAP_GPIO_setOutputLowOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction up
		}
		else
		{
			MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction down
		}
		int j;
		for (j = 0; j < STEPS_PER_HALF_SPACE; j++) {
			stepY();
		}
		MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
		_delay_cycles(MOTOR_MOVE_DELAY);
	}
	if (xdir != 0)
	{
		if (xdir > 0)
		{
			MAP_GPIO_setOutputLowOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction right
		}
		else
		{
			MAP_GPIO_setOutputHighOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction left
		}
		int j;
		for (j = 0; j < STEPS_PER_HALF_SPACE; j++) {
			stepX();
		}
		MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
		_delay_cycles(MOTOR_MOVE_DELAY);
	}
}

void moveToButtons()
{
	MAP_GPIO_setOutputHighOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction left
	MAP_GPIO_setOutputHighOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction down

	// Home on X
	MAP_GPIO_setOutputHighOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	_delay_cycles(MOTOR_AWAKE_DELAY);
	while (1) {
		__delay_cycles(100);
		int val = MAP_GPIO_getInputPinValue(X_HOMING_BUTTON_PORT, X_HOMING_BUTTON_PIN);
		if (val == GPIO_INPUT_PIN_LOW)
		{
			break;
		}
		stepX();
	}
	MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	_delay_cycles(MOTOR_MOVE_DELAY);

	// Home on Y
	MAP_GPIO_setOutputHighOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_AWAKE_DELAY);
	while (1) {
		__delay_cycles(100);
		int val = MAP_GPIO_getInputPinValue(Y_HOMING_BUTTON_PORT, Y_HOMING_BUTTON_PIN);
		if (val == GPIO_INPUT_PIN_LOW)
		{
			break;
		}
		stepY();

	}
	MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_MOVE_DELAY);

	MAP_GPIO_setOutputLowOnPin(X_DIR_PORT, X_DIR_PIN);	// set direction right
	MAP_GPIO_setOutputLowOnPin(Y_DIR_PORT, Y_DIR_PIN);	// set direction up

	// Move to column A (x-axis)
	MAP_GPIO_setOutputHighOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	_delay_cycles(MOTOR_AWAKE_DELAY);
	int i;
	for (i = 0; i < STEPS_COLUMN_A; i++) {
		stepX();
	}
	MAP_GPIO_setOutputLowOnPin(X_SLEEP_PORT, X_SLEEP_PIN);
	_delay_cycles(MOTOR_MOVE_DELAY);

	// Move to row 1 (y-axis)
	MAP_GPIO_setOutputHighOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);
	_delay_cycles(MOTOR_AWAKE_DELAY);
	for (i = 0; i < STEPS_ROW_ONE; i++) {
		stepY();
	}
	MAP_GPIO_setOutputLowOnPin(Y_SLEEP_PORT, Y_SLEEP_PIN);

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

	move(movement, engage, FALSE);
}

void process_moves(piece_movement move1, piece_movement move2)
{
	int move1Valid = move1.rStart != -1 && move1.cStart != -1 ? TRUE : FALSE;
	int move2Valid = move2.rStart != -1 && move2.cStart != -1 ? TRUE : FALSE;
	if (move1Valid)
	{
		// use corner if knight or capture
		int useCorner = (move1.rEnd != move1.rStart && move1.cEnd != move1.cStart
				&& ABSVAL(move1.rEnd - move1.rStart) != ABSVAL(move1.cEnd - move1.cStart))
				|| (move1.rEnd == -1 && move1.cEnd == -1) ? TRUE : FALSE;
		move(move1, TRUE, useCorner);
	}
	if (move2Valid)
	{
		// use corner if knight or castle
		int useCorner = (move2.rEnd != move2.rStart && move2.cEnd != move2.cStart
				&& ABSVAL(move2.rEnd - move2.rStart) != ABSVAL(move2.cEnd - move2.cStart))
				|| (move1.rEnd != -1 && move1.cEnd != -1) ? TRUE : FALSE;
		move(move2, TRUE, useCorner);
	}
}

void move(piece_movement movement, int engage, int useCorner) {
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
	}

	int useCornerUpperLeft = movement.rStart <= 3 ? TRUE : FALSE;
	if (useCorner) {
		if (useCornerUpperLeft) {
			// move to upper right corner
			moveHalfTile(1, 1);
		} else {
			// move to lower left corner
			moveHalfTile(-1, -1);
		}
	}

	if (movement.rEnd == -1 && movement.cEnd == -1)
	{
		int captureRow = gCaptureIndex / 2;
		int captureOffset = gCaptureIndex % 2;
		int isLastCapture = gCaptureIndex == 14 ? TRUE : FALSE;
		if (isLastCapture) {
			captureRow--;	// don't go beyond top edge
		}
		x_move = 7 - movement.cStart;
		y_move = captureRow - movement.rStart;
		if (useCorner && !useCornerUpperLeft) {
			// adjust for corner difference
			y_move++;
			x_move++;
		}

		// move to edge
		moveX(x_move);
		moveY(y_move);

		if (isLastCapture) {
			moveHalfTile(1, 1);
			disengageMagnet();
			moveHalfTile(-1, -1);
		} else {
			if (captureOffset == 0) {
				moveHalfTile(1, -1);
				disengageMagnet();
				moveHalfTile(-1, 1);
			} else {
				moveHalfTile(1, 0);
				disengageMagnet();
				moveHalfTile(-1, 0);
			}
		}

		if (useCorner) {
			// go to center of the rightmost column, whatever row has captured piece
			moveHalfTile(-1, -1);
		}

		// set cursor to dest
		gTableCursor.r = captureRow;
		gTableCursor.c = 7;
		gCaptureIndex++;
	}
	else
	{
		// Compute position difference
		x_move = movement.cEnd - movement.cStart;
		y_move = movement.rEnd - movement.rStart;

		// move the piece to the destination
		if (x_move == 0) {
			moveY(y_move);
		} else if (y_move == 0) {
			moveX(x_move);
		} else if (ABSVAL(x_move) == ABSVAL(y_move)) {
			// move in diagonal
			moveXY(x_move, y_move);
		} else {
			// move sequentially
			moveY(y_move);
			moveX(x_move);
		}

		if (useCorner) {
			// move to center of square
			if (useCornerUpperLeft) {
				moveHalfTile(-1, -1);
			} else {
				moveHalfTile(1, 1);
			}
		}

		if (engage) {
			disengageMagnet();
		}

		// set cursor to destination
		gTableCursor.r = movement.rEnd;
		gTableCursor.c = movement.cEnd;
	}
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
