/*
 * board_state.h
 *
 * Deals with reading from sensor network and determining
 * when a piece movement has been made.
 *
 *  Created on: Oct 18, 2016
 *      Author: Jay
 */

#ifndef BOARD_STATE_H_
#define BOARD_STATE_H_

/** Defines **/
#define BOARD_ROWS 8
#define BOARD_COLS 8
#define MOVES_BUFFER_LENGTH 8
#define MOVES_CHANGE_THRESHOLD 10

/** Typedefs **/
typedef struct {
	// row and col of piece location
	short r;
	short c;
	// flag for direction of change (piece coming off or on)
	short dir;

} piece_change;

typedef struct {
	// row and col of piece start location
	short rStart;
	short cStart;
	// row and col of piece end location
	short rEnd;
	short cEnd;
} piece_movement;

typedef struct {
	// most recent valid state
	short validState[BOARD_ROWS][BOARD_COLS];
	// buffer for current state of board
	short currentState[BOARD_ROWS][BOARD_COLS];
	// buffer for most recent read straight from sensor network
	short nextState[BOARD_ROWS][BOARD_COLS];
	// counters for each board space of number of frames state has changed
	short changeStateCounter[BOARD_ROWS][BOARD_COLS];

	// keep track of moves player has made
	piece_change moveList[MOVES_BUFFER_LENGTH];
	short moveListIndex;
} chess_board;

/** Globals **/
chess_board gBoardState;

/** Functions **/
// do any setup to read from the sensor network
void initSensors();

// compares nextState against currentState and update changeStateCounter
void updateChangeStateCounter();

// for changes that meet threshold, record move in moveList and update currentState and moveList
void updateMoveList();

// in the invalid state, compare currentState against validState and return true if identical
bool updateForBoardReset();

// read from sensor network into nextState
void readNextState();

#endif /* BOARD_STATE_H_ */
