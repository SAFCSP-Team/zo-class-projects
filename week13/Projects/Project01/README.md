# ELF Parser

Simple `readelf` dump tool for 64-bit relocatable object files.

## Build

```
gcc -o elfparser src/*.c
```

## Usage

```
./elfparser <file.o>
./elfparser -c <file.o>     # also dump section contents in hex
```

## Example

```
gcc -c main.c -o main.o
./elfparser main.o
```