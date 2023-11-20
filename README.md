# Crash Bash
This repository is an ongoing decompilation project for the PSX game Crash Bash. To discuss about Crash Bash hacking and reverse engineering, join our Discord: [![Discord](https://img.shields.io/badge/chat-Discord-blue)](https://discord.gg/WwHYE4j9V9)

## Setup
This project is meant to be used as a submodule of the psx modding toolchain. To get started, clone the [toolchain repo](https://github.com/mateusfavarin/psx-modding-toolchain), follow the instructions in their [README](https://github.com/mateusfavarin/psx-modding-toolchain/blob/main/README.md), and then go to the directory `psx-modding-toolchain/games/` and clone the repo there.

## Strategy
This project will adopt the [Ship of Theseus](https://en.wikipedia.org/wiki/Ship_of_Theseus) strategy. By taking advantage of [PCSX-Redux](https://github.com/grumpycoders/pcsx-redux/) 8MB memory expansion, we can re-write game functions and load them in memory, while the original game stays intact. Then, we can force the game to call each function we re-wrote, and compare the output of the decompiled function with the original game function.

## Folder Structure
* `build`: here's where your `NTSC-U` version of Crash Bash is supposed to be in. Rename your game to `bash.bin` and drop it in there.
* `decomp`: reverse engineered code. Each folder inside `decomp/` corresponds to a different binary in game being reverse engineered.
* `include`: general header files that works for every binary. Specific header files will be inside the `decomp/` folder for each specific binary.
* `plugins`: psx-modding-toolchain plugin file, currently does nothing for this project.
* `scripts`: multiple scripts that helps the reverse engineering project.
* `symbols`: linker scripts containing the external symbols from in-game variables and functions.

## Progress
### Ballistics
* `bot.c`: fully functional AI for Crashball;
