# chECEkm8

A capstone project by UVa Electrical and Computer Engineers in fall of 2016. Purpose is to create an autonomous chessboard, i.e. one with which the human can play against a computer using conventional pieces.

A demo video demonstrating usage [is here](https://www.youtube.com/watch?v=8bZ6Lw5DjGU).

## Installation

The code is separated into two parts to be deployed on two machines. First, there is C code to be run on the TI MSP432. The `ccs-workspace` folder is meant to be used as the root of a workspace in Code Composer Studio with which the `checkmate` folder can be opened as a project. Second, there is Python 2.7 code to be run on a standard PC running Windows, Mac, Linux, etc. To run the Python code, you must first install the Python chess and serial libraries. Using `pip` the lines are:

```
pip install python-chess
pip install pyserial
```

From there you can run `python game.py`, specifying:

* A path to an .exe containing a UCI compatible chess engine, like Stockfish
* A name of a serial port connected to the MSP microcontroller
