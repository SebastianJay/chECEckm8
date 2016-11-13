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
        else:
            # check if promotion is valid by appending 'q'
            promotion = chess.Move.from_uci(uci + 'q')
            if promotion in board.legal_moves:
                board.push(promotion)
                player_moved = True

        # received an illegal move
        if not player_moved:
            ser.write('\xff\xff')   # write the error code back to player and wait

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

    # Write auxilary moves, e.g. for capture or special move
    if board.is_capture(move):
        capture_tile = ''
        if board.is_en_passant(move):
            uci = str(move)
            if uci[3] == '6':
                capture_tile = uci[2] + '5'
            elif uci[3] == '3':
                capture_tile = uci[2] + '4'
            else:
                raise Exception('unknown en passant command ' + str(move))
        else:
            capture_tile = str(move)[2:4]
        code = encode_uci(capture_tile)
        code = code & 0x8000    # setting MSB tells client to process another move command
        ser.write(code)
    elif board.is_castling(move):
        king_dest = str(move)[2:4]
        rook_uci = ''
        if king_dest == 'c1':
            rook_uci = 'a1d1'
        elif king_dest == 'c8':
            rook_uci = 'a8d8'
        elif king_dest == 'g1':
            rook_uci = 'h1f1'
        elif king_dest == 'g8':
            rook_uci = 'h8f8'
        else:
            raise Exception('unknown castling command ' + str(move))
        code = encode_uci(rook_uci)
        code = code & 0x8000
        ser.write(code)

    # write the main move
    code = encode_uci(str(move))
    ser.write(code)

    node = node.add_variation(move)
    return node

def debug_shell_move(node, board):
    uci = raw_input("Enter move: ").strip()
    move = chess.Move.from_uci(uci)
    if move in board.legal_moves:
        board.push(move)
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
            #node = player_move(node, board)
            #node = computer_move(node, board, engine, 3)
            node = debug_shell_move(node, board)
            print board

            '''
            if board.is_game_over(): break
            node = computer_move(node, board, engine, DIFFICULTY)
            print board
            '''

        print board
        print board.fen()

        game.headers["Result"] = board.result()
        print game
        break

if __name__ == '__main__':
    main()
