'''
    A.J. Varshneya
    ajv4dg@virginia.edu

    utility.py
'''

import chess

IGNORE_TILES = [
    'd4',        # TODO fix tiles
]

RANK_MAP = {
    'a' : 0,
    'b' : 1,
    'c' : 2,
    'd' : 3,
    'e' : 4,
    'f' : 5,
    'g' : 6,
    'h' : 7
}

# Encodes UCI string into an integer
# e.g. e2e4 -> (12 << 8) | 28
def uci_to_int(move):
    source = RANK_MAP[move[0]] + 8 * (int(move[1]) - 1)
    dest = RANK_MAP[move[2]] + 8 * (int(move[3]) - 1)

    code = (source << 8) | dest
    return code

def int_to_uci(code):
    source = (code >> 8) % 256
    dest = code % 256

    return chess.SQUARE_NAMES[source] + chess.SQUARE_NAMES[dest]

# Converts the UCI string into 2 byte string
def encode_uci(move, next_move_pending=False, game_over=False):
    if len(move) == 2:  # just one location -> indicates a capture
        tmp = uci_to_int(move + 'a1')
        i = tmp | 0x00FF  # all 1s in dest represents capture
    elif len(move) == 4:
        i = uci_to_int(move)
    elif len(move) > 4:
        i = uci_to_int(move[4:])
    else:
        raise Exception('unexpected arg in encode_uci')
    if next_move_pending:
        i = i | 0x8000  # setting MSB tells client to process another move
    if game_over:
        i = i | 0x4000  # setting 2nd MSB tells client that game is over
    s = chr((i >> 8) & 0x00FF) + chr(i & 0x00FF)
    return s

# Converts the 2 byte string into UCI string
def decode_uci(s):
    assert len(s) == 2
    i = (ord(s[0]) << 8) | ord(s[1])
    return int_to_uci(i)
