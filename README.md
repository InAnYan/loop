# Loop language

## Description
Loop is a dynamically-typed programming language. It is designed similar to Python, JavaScript, and TypeScript.
This project is made for educational purposes.

This repository contains the Loop compiler and Loop virtual machine.

## Implementation
Loop programs are compiled into bytecode, and then the virtual machine executes this opcode.

- `loopc` is the byte-code compiler written in Python.
- `loopvm` is the VM written in C.

All Loop source files have extension `.loop`. All compiled bytecode files have extension `.code`.
The compiled source is located in the directory `.loop_compiled` in the directory of the source file.

The implementation was evolved from code in Crafting Interpreters by Bob Nystrom.

## Language features
- Math: integers and arithmetics.
- Global and local variables.
- Functions.
- Closures.
- Module system.
- Exceptions.
- Object-oriented programming.

## Status
Currently in active development. There is no builtins and no functions on the data structures (strings, lists, and dictionaries).

## Usage
Currently there is no release packages and the installation is a little bit daunting.

- Firstly, install the compiler: `pip install -e loop_compiler`.
  This command will give you scripts: `loopc.py` - compiler and `looprun.py` - compiler and runner.
- Then you need to compile from source `loopvm` project (is uses CMake and have no external dependencies, so that should be easy).
- After all of that, you should add the `loopvm` executable to `PATH` and also add an environment variable `LOOP_PACKAGES_PATH`, that
  should point to `packages/` directory in this repository.

## In plans
- Add builtins.
- Add new packages.
- Write the compiler in Loop.
- Write a formatter and an LSP server.

## Some info about spec
Names starting with "_" are internal and they can be redefined. The only thing that user can create is "_" variable (ignore).