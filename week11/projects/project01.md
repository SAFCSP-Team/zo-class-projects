# JSON and YAML Project
A C program that writes the same data to a JSON file and a YAML file.

## Overview
The program has two functions:
1. `json_fun()` - Generates a JSON file.
2. `yaml_fun()` - Generates a YAML file.

Both functions take the same data from a C struct and write it in each format's syntax.

## Example
Data in C
```c
student_t s = {"Sara", 21, {"C", "Linux"}};
```
Command
```
./program
```
Output `student.json`
```json
{
  "name": "Sara",
  "age": 21,
  "courses": ["C", "Linux"]
}
```
Output `student.yaml`
```yaml
name: Sara
age: 21
courses:
  - C
  - Linux
```

## How It Works
- The data is stored in a C struct
- `json_fun()` opens a `.json` file and writes the data using braces, quoted keys, commas and square brackets for lists
- `yaml_fun()` opens a `.yaml` file and writes the data as `key: value` lines, using indentation and `-` for list items
- Each function closes its file and returns whether the write succeeded
