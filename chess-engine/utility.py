'''
    A.J. Varshneya
    ajv4dg@virginia.edu
    
    utility.py
'''

import chess

DIFFICULTY = 2

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
