'''
    A.J. Varshneya
    ajv4dg@virginia.edu

    game.py
'''

import chess
import chess.uci
import chess.pgn

import time
import argparse
import sys

from utility import *
from comm import Comm

# Waits for player move from serial input, then makes the move
def player_move(node, board):
    # Obtain the player's move by continuously polling
    player_moved = False
    ser = Comm.getSerial()
    while not player_moved:
        # throttle the read requests
        time.sleep(0.05)

        # Read from serial buffer
        received = ''
        while ser.inWaiting() > 0:
            received += ser.read(2)

        if len(received) == 0:
            continue    # did not receive anything

        uci = decode_uci(received)

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
    ser = Comm.getSerial()
    code = encode_uci(str(move))
    ser.write(code)

    node = node.add_variation(move)
    return node

def main():
    # Parse command line args
    parser = argparse.ArgumentParser(description='Connect to MSP and play chess.')
    parser.add_argument('-e', '--engine', default='./stockfish-7-64',
        help='Path to a UCI compatible chess engine exe')
    parser.add_argument('-p', '--port', default='/dev/cu.usbmodemM4321001',
        help='Name of a COM port connected to (MSP) client')

    args = vars(parser.parse_args(sys.argv[1:]))
    Comm.setPort(args['port'])

    # Setup
    board = chess.Board()
    engine = chess.uci.popen_engine(args['engine'])
    engine.uci()

    while True:
        board.reset()

        # Create a game and set headers.
        game = chess.pgn.Game()
        game.headers["Black"] = engine.name
        game.headers["White"] = engine.name

        game.setup(board)
        node = game

        print board

        while True:
            if board.is_game_over(): break
            #node = computer_move(node, board, engine, 3)
            node = player_move(node, board)
            print board

            if board.is_game_over(): break
            node = computer_move(node, board, engine, DIFFICULTY)
            print board

        print board
        print board.fen()

        game.headers["Result"] = board.result()
        print game
        break

if __name__ == '__main__':
    main()
