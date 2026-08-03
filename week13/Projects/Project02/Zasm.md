# Tokenizer & Syntax Highlighter

## Description
Build a tokenizer and syntax highlighter for the **Zasm** assembly language project.

## New Features & Additions

### 1. Processing Handler API
* **`najd_string_process_with_handler`**: A flexible function that processes the input string by accepting the string object, readers table, and event callbacks (handler) to emit matched tokens.

### 2. Assembly Readers & Tokens
* **`identifier`**: Parses standard assembly instructions (e.g., `ADD`, `MOV`, `INT`) and registers (e.g., `R0`, `R1`, `R2`).
* **`comma`**: A dedicated reader for capturing operand separating commas (`,`) using lookahead: `najd_string_peek(str, 1) == ','`.
* **`number`**: Parses immediate numerical values and constants.
* **`space`**: Captures spaces, tabs, and newlines (`\n`) to preserve layout.

### 3. Color Mapping (ANSI Codes)
* **ID 1 (`number`)**: Yellow (`\033[1;33m`)
* **ID 2 (`identifier`)**: Cyan (`\033[1;36m`)
* **ID 3 (`space`)**: Default (`\033[0m`)
* **ID 4 (`comma`)**: Red (`\033[1;31m`)