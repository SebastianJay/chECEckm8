#include "driverlib.h"
#include "msp.h"

#include "uart_comm.h"

// Constants
#define XMOVE 500
#define YMOVE 500

void demo() {
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
}

void direction_left() {
	P3OUT |= BIT0;
}

void direction_right() {
	P3OUT &= ~BIT0;
}

void direction_up() {
	P2OUT |= BIT6;
}

void direction_down() {
	P2OUT &= ~BIT6;
}


void init_motor_control() {
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
}

void step_x() {
	int i;
	P2OUT |= BIT5; // ON
	for(i=500; i>0; i--);
	P2OUT &= ~BIT5; // OFF
	for(i=2500; i>0; i--);
}

void move_x(int num_steps) {
	int i;
	for (i = 0; i < num_steps; i++) {
		step_x();
	}
}

void step_y() {
	int i;
	P2OUT |= BIT7; // ON
	for(i=500; i>0; i--);
	P2OUT &= ~BIT7; // OFF
	for(i=2500; i>0; i--);
}

void move_y(int num_steps) {
	int i;
	for (i = 0; i < num_steps; i++) {
		step_y();
	}
}

void move_piece(int src, int dest) {

}

void move_cursor(int * cursor, int destination) {
	// Compute rank/file
	int cursor_rank = *cursor % 8;
	int cursor_file = *cursor / 8;
	int destination_rank = destination % 8;
	int destination_file = destination / 8;

	// Compute position difference
	int x_move = destination_rank - cursor_rank;
	int y_move = destination_file - cursor_file;

	// Set x direction
	if (x_move < 0) {
		x_move *= -1;
		direction_left();
	}
	else {
		direction_right();
	}

	// Move in x
	move_x(XMOVE);

	// Set y direction
	if (y_move < 0) {
		y_move *= -1;
		direction_down();
	} else {
		direction_up();
	}

	// Move in y
	move_y(YMOVE);

	*cursor = destination;
}

void main()
{
	// Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;

	// Setup clock
	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
	MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	// Init motor control pins
	init_motor_control();

	// Init cursor
	int cursor = 0;
	int destination = 9;

	int i = 0;

	move_cursor(&cursor, destination);
	while (i < 1000000) i++;

	destination = 37;
	move_cursor(&cursor, destination);
	while (i < 1000000) i++;

//	destination = 9;
//	move_cursor(&cursor, destination);

	while (1);
}

