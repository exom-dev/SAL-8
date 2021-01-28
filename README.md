# About <a href="https://cmake.org/cmake/help/v3.15"><img align="right" src="https://img.shields.io/badge/CMake-3.15-BA1F28?logo=CMake" alt="CMake 3.15" /></a><a href="https://en.wikipedia.org/wiki/C_(programming_language)"><img align="right" src="https://img.shields.io/badge/C-99-A5B4C6?logo=data:image/svg+xml;base64,PHN2ZyByb2xlPSJpbWciIHZpZXdCb3g9IjAgMCAyNCAyNCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48dGl0bGU+QysrIGljb248L3RpdGxlPjxwYXRoIGZpbGw9IiNmZmZmZmYiIGQ9Ik0yMi4zOTMgNmMtLjE2Ny0uMjktLjM5OC0uNTQzLS42NTItLjY5TDEyLjkyNS4yMmMtLjUwOC0uMjkzLTEuMzM5LS4yOTMtMS44NDcgMEwyLjI2IDUuMzFjLS41MDguMjkzLS45MjMgMS4wMTMtLjkyMyAxLjZ2MTAuMThjMCAuMjk0LjEwNC42Mi4yNzEuOTEuMTY3LjI5LjM5OC41NDMuNjUyLjY4OWw4LjgxNiA1LjA5MWMuNTA4LjI5MyAxLjMzOS4yOTMgMS44NDcgMGw4LjgxNi01LjA5MWMuMjU0LS4xNDYuNDg1LS4zOTkuNjUyLS42ODlzLjI3MS0uNjE2LjI3MS0uOTFWNi45MWMuMDAyLS4yOTQtLjEwMi0uNjItLjI2OS0uOTF6TTEyIDE5LjEwOWMtMy45MiAwLTcuMTA5LTMuMTg5LTcuMTA5LTcuMTA5UzguMDggNC44OTEgMTIgNC44OTFhNy4xMzMgNy4xMzMgMCAwIDEgNi4xNTYgMy41NTJsLTMuMDc2IDEuNzgxQTMuNTY3IDMuNTY3IDAgMCAwIDEyIDguNDQ1Yy0xLjk2IDAtMy41NTQgMS41OTUtMy41NTQgMy41NTVTMTAuMDQgMTUuNTU1IDEyIDE1LjU1NWEzLjU3IDMuNTcgMCAwIDAgMy4wOC0xLjc3OGwzLjA3NyAxLjc4QTcuMTM1IDcuMTM1IDAgMCAxIDEyIDE5LjEwOXoiLz48L3N2Zz4=" alt="C 99" /></a>

**SAL-8** is an interpreted 8-bit simple assembly-like language designed to be embedded in games or other projects. More specifically, it was created for games which revolve around solving programming challenges. 

This is a cross-platform C99 implementation of SAL-8, in the form of a library. A simple `main.c` file for generating an executable is also included.

# Introduction

SAL-8 is an 8-bit language. Therefore, 8-bit numbers (0-255) are the only available data type.

All of the instructions are executed by a virtual machine. Each virtual machine gives the programmer access to two types of resources: a stack, and multiple registers.

- a **register** simply holds an 8-bit number
- the **stack** can hold multiple 8-bit numbers, but only the last one is directly accessible.

The registers can be directly referenced, while the stack can be manipulated via the `PUSH` and `POP` operations.

The number of registers and the stack size can be specified when creating a virtual machine. However, the usual values are **4 registers** and a **stack size** of **8**. These usual values are also used by the provided `main.c` file.

There is also a special register called `CMP`, which stores the result of the last `CMP` operation:

- **0**, if the operands are equal
- **1**, if the left operand is below the right one
- **2**, if the left operand is above the right one

## Running the executable

To run a SAL-8 file with the executable, simply pass the path to the file as the first argument.

```shell
sal8 <FILE>
```

# Syntax

#### Instructions

Each instruction is written on a separate line, and has the following form.

```
<OP> [LEFT] [, RIGHT]
```

The list of all operations is listed in a table below.

#### Comments

Comments are preceded by the `#` character.

```shell
# Single-line comment.
```

#### Labels 
Labels can be created in the following way.

```
<label_name>:
```

To refer to a label, simply write the label name without `:`.

#### Registers

To refer to a register, use the `Rn` syntax, where `n` is the register number (*e.g.* `R0`, `R3`).

## Operation Codes

**Legend:**
- **R** - register
- **N** - 8-bit number (0-255)
- **L** - label

The result is stored in the left register (if applicable).

|  OP  | Left | Right |                                                              Description                                                             |
|:----:|:----:|:-----:|--------------------------------------------------------------------------------------------------------------------------------------|
| ADD  |   R  |  R/N  | Adds the right number/register to the left register                                                                                  |
| AND  |   R  |  R/N  | Applies the bitwise AND operator on the left register and the right number/register                                                  |
| CMP  |  R/N |  R/N  | Compares a register/number value with another register/number value, and stores the result in the CMP register                       |
| DIV  |   R  |  R/N  | Divides the left register value by the right number/register value                                                                   |
| IN   |   R  |   -   | Reads an 8-bit number from STDIN (or IO interface), and stores it in the left register (if there is no more input, the VM is halted) |
| JA   |   L  |   -   | Checks the CMP register, and jumps to the specified label if the left operand is above the right                                     |
| JAE  |   L  |   -   | Similar to JA, but also jumps if the two operands are equal                                                                          |
| JB   |   L  |   -   | Similar to JA, but jumps only if the first operand is below the first                                                                |
| JBE  |   L  |   -   | Similar to JB, but also jumps if the two operands are equal                                                                          |
| JE   |   L  |   -   | Similar to JA, but jumps only if the operands are equal                                                                              |
| JMP  |   L  |   -   | Jumps to the specified label                                                                                                         |
| MOD  |   R  |  R/N  | Applies the mod operator between the left register and right number/register                                                         |
| MOV  |   R  |   R   | Copies the right register value in the left register                                                                                 |
| MUL  |   R  |  R/N  | Multiplies the left register value by the right number/register value                                                                |
| NOT  |   R  |   -   | Applies the bitwise NOT operator on the left register value                                                                          |
| OR   |   R  |  R/N  | Applies the bitwise OR operator on the left register value and right number/register value                                           |
| OUT  |  R/N |   -   | Writes the left register/number value to STDOUT (or IO interface)                                                                    |
| SHL  |   R  |  R/N  | Applies the left shift operator on the left register value, with the right register/number value as the shift count                  |
| SHR  |   R  |  R/N  | Similar to SHL, but shifts to the right                                                                                              |
| POP  |   R  |   -   | Pops the stack and stores the popped element in the left register                                                                    |
| PUSH |  R/N |   -   | Pushes the left register/number value onto the stack                                                                                 |
| SUB  |   R  |  R/N  | Subtracts the right register/number value from the left register                                                                     |
| XOR  |   R  |  R/N  | Applies the bitwise XOR operator on the left register value and the right register/number value                                      |

# Examples

#### Echo

Reads numbers and outputs them.

```asm
_loop:
IN   R0        # Read a number and store it in the first register. If there is no more input, or the input is invalid, the VM will be halted.
OUT  R0        # Output the number stored in the first register.
JMP  _loop     # Jump back.
```

#### Count

Reads a number `n` and prints all numbers from `n` to 0 (inclusive).

```asm
IN  R0
ADD R0, 1     # The number is first subtracted, so add 1 to the initial number.

loop:
SUB R0, 1     # 0 needs to be included. Subtracting 1 from 0 gives 255. So, first subtract, print, and then check.
OUT R0
CMP R0, 0
JA  loop
```

#### Reverse

Reads 8 numbers and prints them in reverse order.

```asm
MOV R0, 0

_input_loop:
IN   R1
PUSH R1         # Read each number and push it onto the stack.
ADD  R0, 1
CMP  R0, 8
JB   _input_loop

MOV R0, 8

_output_loop:
POP R1          # Pop each number from the stack and output it.
OUT R1
SUB R0, 1
CMP R0, 0
JA  _output_loop
```

# License <a href="https://github.com/exom-dev/SAL-8/blob/master/LICENSE"><img align="right" src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License: MIT"></a>

This project was created by [The Exom Developers](https://github.com/exom-dev). It is licensed under the [MIT](https://github.com/exom-dev/SAL-8/blob/master/LICENSE) license.