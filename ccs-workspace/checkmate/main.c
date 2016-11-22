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

	/** UART debugging **/
	//initUART();
	//helloWorldSend();
	//helloWorldReceive();
	//debugGameLoop();

	/** Piece sensing debugging **/
	//initSensors();
//	while (1)
//	{
//		;
//		readNextState();
//		chess_board local = gBoardState;
//		;
//		_delay_cycles(5000);
//	}

	//MAP_Interrupt_enableMaster();
    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
