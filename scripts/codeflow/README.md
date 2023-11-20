# Function Call Analyzer
This script analyzes the assembly instructions from one frame of your game and outputs a text file with every function call the script could find. Symbols are automatically applied to the output in order to make it easier to understand the code flow of your game.

# Getting the assembly trace from your game
* Start `PCSX-Redux` with logging enabled by passing the following command line flags: `-logfile log.txt`
* Start up your game and insert a breakpoint in the first instruction of the game loop in the frame you want to analyze. Unpause the game and wait for the breakpoint to hit.
* In the drop-down menu select `Debug -> CPU -> Show Assembly`, and enable `CPU Trace`. Click on the `Resume` button.
* Once the breakpoint hits again, disable `CPU Trace` and close the emulator (`File -> Quit`).
* A log file `log.txt` should be created in the emulator directory, copy it into this `codeflow` folder.

# Getting the symbols from your Ghidra project
Open your Ghidra project, and in the drop-down menu select `Window -> Python`. Copy/paste the code from `ghidra.py` into the python interpreter. A file named `ghidra_functions.txt` will be created inside your `Ghidra` directory, simply move the created file into this `codeflow` folder.

# Running
Open the terminal and call `python main.py`. The script will look for the `ghidra_functions.txt` and `log.txt` from the current directory and create an `output.txt` file.