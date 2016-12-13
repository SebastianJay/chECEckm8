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
    # Prevent moves to broken tiles
    legal_moves = list(board.generate_legal_moves())
    restricted_moves = [move for move in legal_moves if move.uci()[2:4] not in IGNORE_TILES]

    # Compute response
    engine.position(board)
    move, ponder = engine.go(searchmoves=restricted_moves, depth=difficulty)

    print "Stockfish move: " + str(move)
    write_move_to_serial(board, move)
    board.push(move)
    node = node.add_variation(move)
    return node

def write_move_to_serial(board, move):
    ser = Comm.getSerial()
    # push move temporarily to see if it causes game over
    board.push(move)
    game_over = board.is_game_over()
    board.pop()

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
        code = encode_uci(capture_tile, True, game_over)
        if game_over:
            game_over = False   # prevent next move from setting bit
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
        code = encode_uci(rook_uci, True, game_over)
        if game_over:
            game_over = False   # prevent next move from setting bit
        ser.write(code)

    # write the main move
    code = encode_uci(str(move), False, game_over)
    ser.write(code)

def debug_shell_move(node, board, write_to_com=False):
    while True:
        uci = raw_input("Enter move: ").strip()
        move = chess.Move.from_uci(uci)
        if move in board.legal_moves:
            if write_to_com:
                write_move_to_serial(board, move)
            board.push(move)
            node = node.add_variation(move)
            break
        else:
            print 'Move not valid!'
    return node

def main():
    # Parse command line args
    parser = argparse.ArgumentParser(description='Connect to MSP and play chess.')
    parser.add_argument('-e', '--engine', default='./stockfish-7-64',
        help='Path to a UCI compatible chess engine exe')
    parser.add_argument('-p', '--port', default='/dev/cu.usbmodemM4321001',
        help='Name of a COM port connected to (MSP) client')
    parser.add_argument('-d', '--depth', default='1',
        help='Integer depth of search space for computer move')

    args = vars(parser.parse_args(sys.argv[1:]))
    Comm.setPort(args['port'])

    # Setup
    board = chess.Board()
    engine = chess.uci.popen_engine(args['engine'])
    engine.uci()
    difficulty = int(args['depth'])

    board.reset()

    # Create a game and set headers.
    game = chess.pgn.Game()
    game.headers["Black"] = engine.name
    game.headers["White"] = engine.name

    game.setup(board)
    node = game

    print board

    while True:
        node = player_move(node, board)
        print board
        # player won game, so send a game over signal
        if board.is_game_over():
            ser = Comm.getSerial()
            ser.write('\x40\x00')   # 2nd MSB set; empty move indicates nothing to do client side
            break

        node = computer_move(node, board, engine, difficulty)
        #node = debug_shell_move(node, board, True)
        print board
        if board.is_game_over():
            break

    print board
    print board.fen()

    game.headers["Result"] = board.result()
    print game

if __name__ == '__main__':
    main()
