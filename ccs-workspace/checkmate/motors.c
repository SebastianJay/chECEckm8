/*
 * motors.c
 *
 *  Created on: Nov 6, 2016
 *      Author: Jay
 */

#include "driverlib.h"
#include "motors.h"
#include "board_state.h"

/*
 * Timer_A PWM Configuration Parameter
 * PWM is used to control the servo
 */
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10,
		SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_1,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
		SERVO_PULSE_WIDTH_1
};

void initMotors()
{
	// clock source
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // choose GPIO pin for PWM
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /*
    // TODO understand how to remap pins, so we can possibly change PWM pin from 2.4
    // Port mapper configuration register
	const uint8_t port_mapping[] =
	{
			//Port P2:
			PM_TA0CCR0A, PM_TA0CCR0A, PM_TA0CCR0A, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE
	};
	MAP_PMAP_configurePorts((const uint8_t *) port_mapping, P2MAP, 1, PMAP_DISABLE_RECONFIGURATION);
	*/

    // Configure Timer A PWM
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    // Configure motor driver pins
	// X AXIS
	P2DIR |= BIT5; // Motor driver1 STEP, p2.5
	P2OUT &= ~BIT5;

	P3DIR |= BIT0; // Motor driver1 DIR, p3.0, LOW -> RIGHT, HIGH -> LEFT
	P3OUT &= ~BIT0; // Init RIGHT

	// Y AXIS
	P2DIR |= BIT7; // Motor driver2 STEP, p2.7
	P2OUT &= ~BIT7;

	P2DIR |= BIT6; // Motor driver2 DIR, p2.6, LOW -> DOWN, HIGH -> UP
	P2OUT &= BIT6; // Init UP

	// init globals
	gTableCursor.r = 0;
	gTableCursor.c = 0;
}

void debugMotorDemo() {
	/*
	uint32_t distance = 0;
	uint32_t i = 0;
	while (1) {
		P2OUT |= BIT5; // ON
		P2OUT |= BIT7; // ON
		for(i=500; i>0; i--);
		P2OUT &= ~BIT5; // OFF
		P2OUT &= ~BIT7; // OFF
		for(i=2500; i>0; i--);

		if (distance % 500 == 0) {
			P3OUT ^= BIT0;
		}

		if (distance % 500 == 0) {
			P2OUT ^= BIT6;
		}

		distance++;
	}
	*/

	//gTableCursor.r = 1;
	//gTableCursor.c = 1;

	piece_movement m;
	m.rStart = 0;
	m.cStart = 0;
	m.rEnd = 1;
	m.cEnd = 1;
	movePiece(m);

	m.rStart = 1;
	m.cStart = 1;
	m.rEnd = 0;
	m.cEnd = 0;
	movePiece(m);
}

void debugServoLoop()
{
	while(1)
	{
		int i;
		engageMagnet();
		for (i = 0; i < 1000000; i++);
		disengageMagnet();
		for (i = 0; i < 1000000; i++);
	}
}

void step_x() {
	int i;
	P2OUT |= BIT5; // ON
	for(i=X_STEP_ON; i>0; i--);
	P2OUT &= ~BIT5; // OFF
	for(i=X_STEP_OFF; i>0; i--);
}

void move_x(int num_spaces) {
	if (num_spaces < 0) {
		num_spaces = -num_spaces;
		P3OUT |= BIT0;	// set direction left
	} else {
		P3OUT &= ~BIT0;	// set direction right
	}
	int i;
	int j;
	for (i = 0; i < num_spaces; i++) {
		for (j = 0; j < STEPS_PER_SPACE; j++) {
			step_x();
		}
	}
}

void step_y() {
	int i;
	P2OUT |= BIT7; // ON
	for(i=Y_STEP_ON; i>0; i--);
	P2OUT &= ~BIT7; // OFF
	for(i=Y_STEP_OFF; i>0; i--);
}

void move_y(int num_spaces) {
	if (num_spaces < 0) {
		num_spaces = -num_spaces;
		P2OUT &= ~BIT6;	// set direction down
	} else {
		P2OUT |= BIT6;	// set direction up
	}
	int i;
	int j;
	for (i = 0; i < num_spaces; i++) {
		for (j = 0; j < STEPS_PER_SPACE; j++) {
			step_y();
		}
	}
}

void movePiece(piece_movement movement) {
	// Compute difference from cursor to start location
	int x_move;
	int y_move;
	x_move = movement.cStart - gTableCursor.c;
	y_move = movement.rStart - gTableCursor.r;

	//disengageMagnet();

	// move to the piece's source
	move_x(x_move);
	move_y(y_move);

	// Compute rank/file
	if (movement.rEnd == -1 && movement.cEnd == -1)
	{
		// TODO move piece to capture space
		return;
	}

	// Compute position difference
	x_move = movement.cEnd - movement.cStart;
	y_move = movement.rEnd - movement.rStart;

	//engageMagnet();
	// TODO move to corner of square

	// move the piece to the destination
	move_x(x_move);
	move_y(y_move);

	// set cursor to destination
	gTableCursor.r = movement.rEnd;
	gTableCursor.c = movement.cEnd;
}

void engageMagnet()
{
	pwmConfig.timerPeriod = SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_1;
	pwmConfig.dutyCycle = SERVO_PULSE_WIDTH_1;
	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
	// could use busy loop so action completes before code proceeds
}

void disengageMagnet()
{
	pwmConfig.timerPeriod = SERVO_DELAY_BETWEEN_PULSE + SERVO_PULSE_WIDTH_2;
	pwmConfig.dutyCycle = SERVO_PULSE_WIDTH_2;
	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
	// could use busy loop so action completes before code proceeds
}
