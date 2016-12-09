#include "driverlib.h"
#include "msp.h"

#include "uart_comm.h"
#include "motors.h"
#include "board_state.h"

void main()
{
	// Stop watchdog timer
    MAP_WDT_A_holdTimer();

	// Setup clock
	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
	MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	/** Servo, motors debugging **/
	//initMotors();
	//debugServoLoop();
	//debugMotorDemo();
	//debugButtonDemo();

	/** UART debugging **/
	//initUART();
	//helloWorldSend();
	//helloWorldReceive();

	/** Piece sensing debugging **/
	initSensors();

	// initialize all interrupts before running main game loop
	//MAP_Interrupt_enableMaster();
	//debugGameLoop();
	while (1)
	{
		signed char status;
		piece_movement move;
		readNextState();
		updateChangeStateCounter();
		updateCurrentState(TRUE);
		status = constructPieceMovement(&move);
		if (status == TRUE && move.cStart == 0)
		{
			;
			char x = 0;
		}
		else if (status == ERROR)
		{
			gBoardState.moveListIndex = 0;	// reset
		}
		_delay_cycles(5000);
	}

    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
