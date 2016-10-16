'''
    A.J. Varshneya
    ajv4dg@virginia.edu
    
    game.py
'''

import chess
import chess.uci
import chess.pgn 

import time

from utility import *
from comm import *

# Waits for player move from serial input, then makes the move
def player_move(node, board):
    # Obtain the player's move
    player_moved = False
    while not player_moved:
        # Get move
        try:
            # Read from serial buffer
            received = ''
            while ser.inWaiting() > 0:
                received += ser.read(5)
            
            ''' TESTING '''
            # code = uci_to_int(received.rstrip())
            ''' TESTING '''

            code = int(received.rstrip())
            uci = int_to_uci(code)
            
        except:
            # Try again ...
            continue

        print "Player move: " + uci
        move = chess.Move.from_uci(uci)

        if move in board.legal_moves:
            board.push(move)
            player_moved = True

    node = node.add_variation(move)
    return node

def computer_move(node, board, engine, difficulty):
    # Compute response
    engine.position(board)
    move, ponder = engine.go(depth=difficulty)
    board.push(move)

    print "Stockfish move: " + str(move)

    # Write move to serial 
    code = uci_to_int(str(move))
    ser.write(str(code))
    
    node = node.add_variation(move)
    return node

def main():
    # Setup
    board = chess.Board()
    engine = chess.uci.popen_engine("./stockfish-7-64")
    engine.uci()

    while True:
        board.reset()

        # Create a game and set headers.
        game = chess.pgn.Game()
        game.headers["Black"] = engine.name
        # game.headers["White"] = engine.name

        game.setup(board)
        node = game
 
        print board

        while True:
            if board.is_game_over(): break
            # node = computer_move(node, board, engine, 3)
            node = player_move(node, board)
            print board

            if board.is_game_over(): break
            node = computer_move(node, board, engine, DIFFICULTY)
            print board

        print board
        print board.fen()

        game.headers["Result"] = board.result()
        print game

if __name__ == '__main__':
    main()
