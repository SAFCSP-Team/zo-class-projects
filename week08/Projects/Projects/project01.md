# Assembler App
A command-line tool that translates ZVM assembly code into a binary program the VM can run.

## Overview
The assembler reads a source file written in assembly, such as `LDI R0, 5`, checks each line, and writes the matching bytes to a binary file that the VM can load.

## Example
Source file `program.zasm`
```
LDI  R0, 5
LDI  R1, 3
ADD  R0, R1, R2
PUSH R2
```
Command
```
./zasm program.zasm program.bin
```
Output `program.bin` (hex)
```
02 00 05 00
02 01 03 00
00 00 01 02
07 02 00 00
```

## How It Works
- The assembler reads the source file line by line, skipping empty lines and comments (`;`)
- Each mnemonic is looked up in the instruction table below to get its opcode
- Registers `R0`–`R3` become `0`–`3`, and immediate values must be `0`–`255`
- Each instruction is encoded as 4 bytes: `opcode, operand 1, operand 2, operand 3`, with unused operands set to `0`
- On an error, the assembler prints the line number and reason, and writes no output

| Mnemonic | Opcode | Operands      |
|----------|--------|---------------|
| ADD      | 0      | reg, reg, reg |
| SUB      | 1      | reg, reg, reg |
| LDI      | 2      | reg, imm      |
| STR      | 3      | reg, mem      |
| LDM      | 4      | reg, mem      |
| LDA      | 5      | reg, reg      |
| STA      | 6      | reg, reg      |
| PUSH     | 7      | reg           |
| POP      | 8      | reg           |
| INC      | 9      | reg           |
| DEC      | 10     | reg           |
| OUT      | 11     | reg, reg, reg |
