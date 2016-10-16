'''
    A.J. Varshneya
    ajv4dg@virginia.edu
    
    tests.py
'''

import unittest
import chess

import utility

class TestUCIToInt(unittest.TestCase):
    
    def test_e2e4(self):
        move = 'e2e4'
        code = utility.uci_to_int(move)
        self.assertEqual(code, 0x0C1C)

    def test_g1e2(self):
        move = 'g1e2'
        code = utility.uci_to_int(move)
        self.assertEqual(code, 0x060C)

    def test_g1d3(self):
        move = 'g1d3'
        code = utility.uci_to_int(move)
        self.assertEqual(code, 0x0613)

    def test_a1a1(self):
        move = 'a1a1'
        code = utility.uci_to_int(move)
        self.assertEqual(code, 0x0000)

    def test_h8h8(self):
        move = 'h8h8'
        code = utility.uci_to_int(move)
        self.assertEqual(code, 0x3f3f)

    def test_a1h8(self):
        move = 'a1h8'
        code = utility.uci_to_int(move)
        self.assertEqual(code, 0x003f)

    def test_h8a1(self):
        move = 'h8a1'
        code = utility.uci_to_int(move)
        self.assertEqual(code, 0x3f00)

class TestIntToUCI(unittest.TestCase):

    def test_e2e4(self):
        move = 'e2e4'
        code = utility.uci_to_int(move)
        uci = utility.int_to_uci(code)
        self.assertEqual(uci, 'e2e4')

    def test_g1e2(self):
        move = 'g1e2'
        code = utility.uci_to_int(move)
        uci = utility.int_to_uci(code)
        self.assertEqual(uci, 'g1e2')

    def test_g1d3(self):
        move = 'g1d3'
        code = utility.uci_to_int(move)
        uci = utility.int_to_uci(code)
        self.assertEqual(uci, 'g1d3')

    def test_a1a1(self):
        move = 'a1a1'
        code = utility.uci_to_int(move)
        uci = utility.int_to_uci(code)
        self.assertEqual(uci, 'a1a1')

    def test_h8h8(self):
        move = 'h8h8'
        code = utility.uci_to_int(move)
        uci = utility.int_to_uci(code)
        self.assertEqual(uci, 'h8h8')

    def test_a1h8(self):
        move = 'a1h8'
        code = utility.uci_to_int(move)
        uci = utility.int_to_uci(code)
        self.assertEqual(uci, 'a1h8')

    def test_h8a1(self):
        move = 'h8a1'
        code = utility.uci_to_int(move)
        uci = utility.int_to_uci(code)
        self.assertEqual(uci, 'h8a1')

if __name__ == '__main__':
    unittest.main(verbosity=2)
