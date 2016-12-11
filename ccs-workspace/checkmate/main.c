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
	initMotors();
	debugMotorDemo();

	/** UART debugging **/
	//initUART();
	//helloWorldSend();
	//helloWorldReceive();
	//MAP_Interrupt_enableMaster();
	//debugGameLoop();

	/** Piece sensing debugging **/
	//initSensors();

	// initialize all interrupts before running main game loop
	//MAP_Interrupt_enableMaster();
	//debugGameLoop();

//	while (1)
//	{
//		signed char status;
//		piece_movement move;
//		readNextState();
//		updateChangeStateCounter();
//		updateCurrentState(TRUE);
//		status = constructPieceMovement(&move);
//		if (status == TRUE)
//		{
//			;
//			char x = 0;
//		}
//		else if (status == ERROR)
//		{
//			gBoardState.moveListIndex = 0;	// reset
//		}
//		_delay_cycles(5000);
//	}

	/** Begin game loop **/
	// initialize everything
	initMotors();
	initSensors();
	initUART();
	MAP_Interrupt_enableMaster();

	// run main loop
	char status = STATUS_NORMAL;
	while (1)
	{
		// read from the piece sensors
		readNextState();
		updateChangeStateCounter();
		if (status == STATUS_NORMAL)
		{
			// update the piece change list
			updateCurrentState(TRUE);

			// parse piece changes into movement
			piece_movement human_move;
			signed char ret;
			ret = constructPieceMovement(&human_move);
			if (ret == TRUE)
			{
				// send movement to server
				send(&human_move);

				// receive responses
				piece_movement response1, response2;
				ret = receive(&response1, &response2);
				if (ret == TRUE || ret == GAMEOVER_WITH_MOVE)
				{
					// execute server response to move pieces - can be one or two moves
					move(response1, TRUE);
					if (response2.rStart != 0xFF && response2.rEnd != 0xFF)
					{
						move(response2, TRUE);
					}

					if (ret == GAMEOVER_WITH_MOVE)
					{
						// player lost
						break;
					}

					// refresh valid state
					copyCurrentStateIntoValid();
				}
				else if (ret == GAMEOVER_NO_MOVE)
				{
					// player won
					break;
				}
				else if (ret == ERROR)
				{
					// server responds that player move is invalid
					setStatusLed(FALSE);
					status = STATUS_INVALID;
				}
			}
			else if (ret == ERROR)
			{
				// move parsing senses invalid move
				setStatusLed(FALSE);
				status = STATUS_INVALID;
			}
		}
		else if (status == STATUS_INVALID)
		{
			// update current state and see if it matches last valid state
			updateCurrentState(FALSE);
			signed char ret;
			ret = isCurrentStateValid();
			if (ret == TRUE)
			{
				setStatusLed(TRUE);
				status = STATUS_NORMAL;
			}
		}
	}

    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
