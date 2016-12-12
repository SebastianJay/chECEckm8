/*
 * board_state.c
 *
 *  Created on: Oct 30, 2016
 *      Author: Jay
 */

#include "board_state.h"
#include "driverlib.h"

void initSensors()
{
	// init status indicator
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
			GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

	// init pins
	MAP_GPIO_setAsOutputPin(MUX_SELECT1_PORT, MUX_SELECT1_PIN);
	MAP_GPIO_setAsOutputPin(MUX_SELECT2_PORT, MUX_SELECT2_PIN);
	MAP_GPIO_setAsOutputPin(MUX_SELECT3_PORT, MUX_SELECT3_PIN);
	MAP_GPIO_setAsOutputPin(MUX_SELECT4_PORT, MUX_SELECT4_PIN);
	MAP_GPIO_setAsInputPin(MUX_READ1_PORT, MUX_READ1_PIN);
	MAP_GPIO_setAsInputPin(MUX_READ2_PORT, MUX_READ2_PIN);
	MAP_GPIO_setAsInputPin(MUX_READ3_PORT, MUX_READ3_PIN);
	MAP_GPIO_setAsInputPin(MUX_READ4_PORT, MUX_READ4_PIN);

	// init status LEDs
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);

	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);

	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

	// init globals
	char r, c, i;
	for (r = 0; r < BOARD_ROWS; r++)
	{
		for (c = 0; c < BOARD_COLS; c++)
		{
			char startState = (r <= 1 || r >= 6) ? 1 : 0;
			gBoardState.validState[r][c] = startState;
			gBoardState.currentState[r][c] = startState;
			gBoardState.nextState[r][c] = startState;
			gBoardState.changeStateCounter[r][c] = 0;
			for (i = 0; i < MOVES_CHANGE_BUFFER_WINDOW; i++)
			{
				gBoardState.changeStateBuffer[r][c][i] = 0;
			}
		}
	}
	gBoardState.moveListIndex = 0;
	gBoardState.changeStateBufferIndex = 0;
}

void readNextState()
{
	int i;
	int j;
	unsigned int test[4];	// debugging
	for (j = 0; j < 4; j++)
	{
		test[j] = 0;
	}
	for (i = 0; i < 16; i++)
	{
		// set the select bits for the mux
		char bit1 = ((i & 0x1) != 0) ? 1 : 0;
		char bit2 = ((i & 0x2) != 0) ? 1 : 0;
		char bit3 = ((i & 0x4) != 0) ? 1 : 0;
		char bit4 = ((i & 0x8) != 0) ? 1 : 0;
		if (bit1) {
			MAP_GPIO_setOutputHighOnPin(MUX_SELECT1_PORT, MUX_SELECT1_PIN);
		} else {
			MAP_GPIO_setOutputLowOnPin(MUX_SELECT1_PORT, MUX_SELECT1_PIN);
		}
		if (bit2) {
			MAP_GPIO_setOutputHighOnPin(MUX_SELECT2_PORT, MUX_SELECT2_PIN);
		} else {
			MAP_GPIO_setOutputLowOnPin(MUX_SELECT2_PORT, MUX_SELECT2_PIN);
		}
		if (bit3) {
			MAP_GPIO_setOutputHighOnPin(MUX_SELECT3_PORT, MUX_SELECT3_PIN);
		} else {
			MAP_GPIO_setOutputLowOnPin(MUX_SELECT3_PORT, MUX_SELECT3_PIN);
		}
		if (bit4) {
			MAP_GPIO_setOutputHighOnPin(MUX_SELECT4_PORT, MUX_SELECT4_PIN);
		} else {
			MAP_GPIO_setOutputLowOnPin(MUX_SELECT4_PORT, MUX_SELECT4_PIN);
		}

		_delay_cycles(1000);	// TODO remove if not needed
		char piece[4];
		char value;
		value = MAP_GPIO_getInputPinValue(MUX_READ1_PORT, MUX_READ1_PIN);
		piece[0] = (value == GPIO_INPUT_PIN_LOW) ? 1 : 0;
		value = MAP_GPIO_getInputPinValue(MUX_READ2_PORT, MUX_READ2_PIN);
		piece[1] = (value == GPIO_INPUT_PIN_LOW) ? 1 : 0;
		value = MAP_GPIO_getInputPinValue(MUX_READ3_PORT, MUX_READ3_PIN);
		piece[2] = (value == GPIO_INPUT_PIN_LOW) ? 1 : 0;
		value = MAP_GPIO_getInputPinValue(MUX_READ4_PORT, MUX_READ4_PIN);
		piece[3] = (value == GPIO_INPUT_PIN_LOW) ? 1 : 0;

		// mux through software
		for (j = 0; j < 4; j++)
		{
			gBoardState.nextState[(i / 8) + (j * 2)][i % 8] = piece[j];
		}

		// debugging
		for (j = 0; j < 4; j++)
		{
			test[j] = test[j] << 1;
			test[j] |= piece[j];
		}
	}
}

void updateChangeStateCounter()
{
	char r, c;
	for (r = 0; r < BOARD_ROWS; r++)
	{
		for (c = 0; c < BOARD_COLS; c++)
		{
			//if (c != 0) {
			//	continue;	//DEBUG
			//}
			char change = gBoardState.nextState[r][c] != gBoardState.currentState[r][c] ? 1 : 0;
			char previousValue = gBoardState.changeStateBuffer[r][c][gBoardState.changeStateBufferIndex];
			if (change == 1 && previousValue == 0)
			{
				gBoardState.changeStateCounter[r][c]++;
			}
			if (change == 0 && previousValue == 1)
			{
				gBoardState.changeStateCounter[r][c]--;
			}
			gBoardState.changeStateBuffer[r][c][gBoardState.changeStateBufferIndex] = change;
		}
	}
	gBoardState.changeStateBufferIndex = (gBoardState.changeStateBufferIndex + 1) % MOVES_CHANGE_BUFFER_WINDOW;
}

void updateCurrentState(char updateMoveList)
{
	char r, c, i;
	for (r = 0; r < BOARD_ROWS; r++)
	{
		for (c = 0; c < BOARD_COLS; c++)
		{
			if (gBoardState.changeStateCounter[r][c] >= MOVES_CHANGE_THRESHOLD)
			{
				if (updateMoveList)
				{
					// log the change in moveList
					gBoardState.moveList[gBoardState.moveListIndex].r = r;
					gBoardState.moveList[gBoardState.moveListIndex].c = c;
					gBoardState.moveList[gBoardState.moveListIndex].dir = 1 - gBoardState.currentState[r][c];
					gBoardState.moveListIndex = (gBoardState.moveListIndex + 1) % MOVES_BUFFER_LENGTH;
				}

				// update the currentState
				gBoardState.currentState[r][c] = 1 - gBoardState.currentState[r][c];
				gBoardState.changeStateCounter[r][c] = 0;
				for (i = 0; i < MOVES_CHANGE_BUFFER_WINDOW; i++)
				{
					gBoardState.changeStateBuffer[r][c][i] = 0;
				}
			}
		}
	}
}

signed char isCurrentStateValid()
{
	char r, c;
	for (r = 0; r < BOARD_ROWS; r++)
	{
		for (c = 0; c < BOARD_COLS; c++)
		{
			if (gBoardState.currentState[r][c] != gBoardState.validState[r][c])
			{
				return FALSE;
			}
		}
	}
	// reset the moveList if any stale data is there
	gBoardState.moveListIndex = 0;
	// board back to last valid state
	return TRUE;
}

void updateCurrentStateWithMove(piece_movement move)
{
	gBoardState.currentState[move.rStart][move.cStart] = 0;
	if (move.rEnd != -1 && move.cEnd != -1)
	{
		gBoardState.currentState[move.rEnd][move.cEnd] = 1;
	}
}

void copyCurrentStateIntoValid()
{
	char r, c;
	for (r = 0; r < BOARD_ROWS; r++)
	{
		for (c = 0; c < BOARD_COLS; c++)
		{
			gBoardState.validState[r][c] = gBoardState.currentState[r][c];
		}
	}
}

void setStatusLed(signed char status)
{
	if (status == TRUE)
	{
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
	}
	else if (status == FALSE || status == ERROR)
	{
		MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
	}
	else if (status == GAMEOVER_NO_MOVE)
	{
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
		MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
	}
	else if (status == GAMEOVER_WITH_MOVE)
	{
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
		MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
		MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
	}
}

signed char constructPieceMovement(piece_movement* move)
{
	char i;
	// information about the piece moving
	// location of first piece coming off board
	signed char rStart1 = -1;
	signed char cStart1 = -1;
	// location of second piece coming off board
	signed char rStart2 = -1;
	signed char cStart2 = -1;
	// location of first piece coming onto board
	signed char rEnd1 = -1;
	signed char cEnd1 = -1;
	// extra flags for castle if it is detected
	char isCastle = FALSE;
	char isCastleFinished = FALSE;

	// loop through all the recognized moves
	for (i = 0; i < gBoardState.moveListIndex; i++)
	{
		char r = gBoardState.moveList[i].r;
		char c = gBoardState.moveList[i].c;

		if (gBoardState.moveList[i].dir == 0)	// piece coming off board
		{
			// if source was not previously set
			if (rStart1 == -1 && cStart1 == -1)
			{
				rStart1 = r;
				cStart1 = c;
			}
			// if second source was not previously set
			else if (rStart2 == -1 && cStart2 == -1)
			{
				rStart2 = r;
				cStart2 = c;
			}
			else
			{
				// no chess move requires three pieces to come off board - seems invalid
				return ERROR;
			}
		}
		else		// piece moving onto the board
		{
			// if destination was not previously set
			if (rEnd1 == -1 && cEnd1 == -1)
			{
				rEnd1 = r;
				cEnd1 = c;
				// continue special logic if two pieces came off board
				if (rStart2 != -1 && cStart2 != -1)
				{
					if (rEnd1 == rStart1 && cEnd1 == cStart1)
					{
						// piece moved to where first piece moved off - adjust start position
						rStart1 = rStart2;
						cStart1 = cStart2;
						break;
					}
					else if (rEnd1 == rStart2 && cEnd1 == cStart2)
					{
						// piece moved to where second moved off, start is ok
						break;
					}
					else
					{
						// piece moved to where neither moved off
						// check for en passant
						if (cEnd1 == cStart1)
						{
							// piece moved to same column as first piece coming off - adjust start position
							rStart1 = rStart2;
							cStart1 = cStart2;
							break;
						}
						else if (cEnd1 == cStart2)
						{
							// piece moved to same column as second coming off, start is ok
							break;
						}
						else
						{
							// check for castle
							if ((rStart1 == 0 || rStart1 == 7) && cStart1 == 4)
							{
								// first piece moved from valid king start point
								isCastle = TRUE;
							}
							else if ((rStart2 == 0 || rStart2 == 7) && cStart2 == 4)
							{
								// second piece moved from valid king start point
								char rtmp = rStart1;
								char ctmp = cStart1;
								rStart1 = rStart2;
								cStart1 = cStart2;
								rStart2 = rtmp;
								cStart2 = ctmp;	// swap so king point is first in ordering
								isCastle = TRUE;
							}
							else
							{
								// nothing else seems possible at this point
								return ERROR;
							}
						}
					}
				}
			}
			else if (isCastle)
			{
				// find the valid king ending point
				char rEnd2 = r;
				char cEnd2 = c;
				if ((rEnd2 == 0 || rEnd2 == 7) && (cEnd2 == 2 || cEnd2 == 6))
				{
					// second piece moving on was king - swap end position
					char rtmp = rEnd1;
					char ctmp = cEnd1;
					rEnd1 = rEnd2;
					cEnd1 = cEnd2;
					rEnd2 = rtmp;
					cEnd2 = ctmp;
				}

				// compare rook and king start and end positions
				if ((rStart1 == 0 && cStart1 == 4 && rStart2 == 0 && cStart2 == 7 && rEnd1 == 0 && cEnd1 == 6 && rEnd2 == 0 && cEnd2 == 5) ||
					(rStart1 == 0 && cStart1 == 4 && rStart2 == 0 && cStart2 == 0 && rEnd1 == 0 && cEnd1 == 2 && rEnd2 == 0 && cEnd2 == 3) ||
					(rStart1 == 7 && cStart1 == 4 && rStart2 == 7 && cStart2 == 7 && rEnd1 == 7 && cEnd1 == 6 && rEnd2 == 7 && cEnd2 == 5) ||
					(rStart1 == 7 && cStart1 == 4 && rStart2 == 7 && cStart2 == 0 && rEnd1 == 7 && cEnd1 == 2 && rEnd2 == 7 && cEnd2 == 3))
				{
					isCastleFinished = TRUE;
					break;
				}
				else
				{
					// not a castle, so this seems invalid
					return ERROR;
				}
			}
			else
			{
				// no non-castle move requires two pieces to come onto board - seems invalid
				return ERROR;
			}
		}
	}

	// if a valid start and end location was produced
	if (rStart1 != -1 && cStart1 != -1 && rEnd1 != -1 && cEnd1 != -1 && !(isCastle && !isCastleFinished))
	{
		// if start same as end (player moved piece off and on)
		if (rStart1 == rEnd1 && cStart1 == cEnd1)
		{
			// ignore those moves and reset moveList
			gBoardState.moveListIndex = 0;
			return FALSE;
		}
		else
		{
			// reset moveList
			gBoardState.moveListIndex = 0;

			// return the movement
			move->rStart = rStart1;
			move->cStart = cStart1;
			move->rEnd = rEnd1;
			move->cEnd = cEnd1;
			return TRUE;
		}
	}

	// otherwise, move not fully formed yet
	return FALSE;
}
