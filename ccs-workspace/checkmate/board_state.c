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
	// init globals
	short r, c, i;
	for (r = 0; r < BOARD_ROWS; r++)
	{
		for (c = 0; c < BOARD_COLS; c++)
		{
			short startState = (r <= 1 || r >= 6) ? 1 : 0;
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

void updateChangeStateCounter()
{
	short r, c;
	for (r = 0; r < BOARD_ROWS; r++)
	{
		for (c = 0; c < BOARD_COLS; c++)
		{
			short change = gBoardState.nextState[r][c] != gBoardState.currentState[r][c] ? 1 : 0;
			short previousValue = gBoardState.changeStateBuffer[r][c][gBoardState.changeStateBufferIndex];
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
	short r, c, i;
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
					gBoardState.moveListIndex++;
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

char isCurrentStateValid()
{
	short r, c;
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

char constructPieceMovement(piece_movement* move)
{
	short i;
	// information about the piece moving
	// location of first piece coming off board
	short rStart1 = -1;
	short cStart1 = -1;
	// location of second piece coming off board
	short rStart2 = -1;
	short cStart2 = -1;
	// location of first piece coming onto board
	short rEnd1 = -1;
	short cEnd1 = -1;
	// extra flags for castle if it is detected
	short isCastle = FALSE;
	short isCastleFinished = FALSE;

	// loop through all the recognized moves
	for (i = 0; i < gBoardState.moveListIndex; i++)
	{
		short r = gBoardState.moveList[i].r;
		short c = gBoardState.moveList[i].c;

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
							short rtmp = rStart1;
							short ctmp = cStart1;
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
			else if (isCastle)
			{
				// find the valid king ending point
				short rEnd2 = r;
				short cEnd2 = c;
				if ((rEnd2 == 0 || rEnd2 == 7) && (cEnd2 == 2 || cEnd2 == 6))
				{
					// second piece moving on was king - swap end position
					short rtmp = rEnd1;
					short ctmp = cEnd1;
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
				}
				else
				{
					// not a castle, so this seems invalid
					return ERROR;
				}
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
