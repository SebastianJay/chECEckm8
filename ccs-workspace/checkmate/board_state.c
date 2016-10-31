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
			if (change == 1 && gBoardState.changeStateBuffer[r][c][gBoardState.changeStateBufferIndex] == 0)
			{
				gBoardState.changeStateCounter[r][c]++;
			}
			if (change == 0 && gBoardState.changeStateBuffer[r][c][gBoardState.changeStateBufferIndex] == 1)
			{
				gBoardState.changeStateCounter[r][c]--;
			}
			gBoardState.changeStateBuffer[r][c][gBoardState.changeStateBufferIndex] = change;
		}
	}
	gBoardState.changeStateBufferIndex = (gBoardState.changeStateBufferIndex + 1) % MOVES_CHANGE_BUFFER_WINDOW;
}

void updateCurrentState(short updateMoveList)
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

short isCurrentStateValid()
{
	short r, c, i;
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
	return TRUE;
}

short constructPieceMovement(piece_movement* move)
{
	short i;
	short rStart = -1;
	short rEnd = -1;
	short cStart = -1;
	short cEnd = -1;
	short isCastle = FALSE;
	for (gBoardState.moveListIndex; i >= 0; i--)
	{
		short r = gBoardState.moveList[i].r;
		short c = gBoardState.moveList[i].c;

		// piece moving onto the board
		if (gBoardState.moveList[i].dir == 1)
		{
			// if destination was not previously set
			if (rEnd != -1 && cEnd != -1)
			{
				rEnd = r;
				cEnd = c;
			}
			else
			{
				short rRook;
				short cRook;
				// check for castling
				if ((r == 0 || r == 7) && (c == 2 && c == 6))
				{
					// move currently examined is king end position
					rRook = rEnd;
					cRook = cEnd;
					rEnd = r;
					cEnd = c;
				}
				else if (!((rEnd == 0 || rEnd == 7) && (cEnd == 2 && cEnd == 6)))
				{
					// multiple pieces came onto board and one was not a valid king landing spot
					//  therefore do not recognize this move as valid
					return ERROR;
				}
				else
				{
					// move currently examined is rook end position
					rRook = r;
					cRook = c;
				}

				// TODO validate king and rook end positions

				isCastle = TRUE;
			}
		}
		else	// piece coming off board
		{
			// if source was not previously set
			if (rStart != -1 && cStart != -1)
			{
				rStart = r;
				cStart = c;
			}
			else
			{
				if (isCastle)
				{
					short rRook;
					short cRook;
					// check for castling
					if ((r == 0 || r == 7) && c == 4)
					{
						// move currently examined is king start position
						rRook = rStart;
						cRook = cStart;
						rStart = r;
						cStart = c;
					}
					else if (!((rStart == 0 || rStart == 7) && cStart == 4))
					{
						// multiple pieces lifted off board and one was not a valid king starting spot
						//  therefore do not recognize this move as valid
						return ERROR;
					}
					else
					{
						// move currently examined is rook start position
						rRook = r;
						cRook = c;
					}

					// TODO validate rook and king start positions

				}
				else
				{
					// piece was lifted at destination, indicating possible capture
					if (rStart == rEnd && cStart == cEnd)
					{
						// use where other piece was lifted
						rStart = r;
						cStart = c;
					}
					else
					{
						// could be en passant, so use correct starting position
						// if current start position is where captured pawn was lifted, use other move
						if (cStart == cEnd)
						{
							rStart = r;
							cStart = c;
						}
					}
				}
			}
		}
	}

	// if a valid start and end location was produced
	if (rStart != -1 && cStart != -1 && rEnd != -1 && cEnd != -1)
	{
		// if start same as end (player moved piece off and on)
		if (rStart == rEnd && cStart == cEnd)
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
			move->rStart = rStart;
			move->cStart = cStart;
			move->rEnd = rEnd;
			move->cEnd = cEnd;
			return TRUE;
		}
	}

	// otherwise, move not fully formed yet
	return FALSE;
}
