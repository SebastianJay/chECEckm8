# Dependencies

* UCI compatible chess engine executable - In our project we use [Stockfish](https://stockfishchess.org/). Binaries of Stockfish can be found [here](https://stockfishchess.org/download/). Once the binary is on local disk, you can reference it with the `--engine` switch when running `game.py`.
* [Driverlib](http://www.ti.com/tool/mspdriverlib) - A software API layer that abstracts away register manipulation to interact with the MSP432's peripherals and modules. Download the MSP432DRIVERLIB zip; locate the `driverlib` folder and drop it into `ccs-workspace`. Make sure that folder is listed in the Includes section for the project (go to Properties -> Build -> MSP432 Compiler -> Include Options -> Add `${WORKSPACE_LOC}/driverlib/MSP432P4xx`)
