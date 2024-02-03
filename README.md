# Loop language

## Description
Loop is a dynamically-typed programming language. It is designed similar to Python, JavaScript, and TypeScript.
This project is made for educational purposes.

## Implementation
Loop programs are compiled into bytecode, and then the virtual machine executes this opcode.

-`loopc` is the byte-code compiler written in Python.
-`loopvm` is the VM written in C.

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