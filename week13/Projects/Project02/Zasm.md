# Tokenizer & Syntax Highlighter

## Description
Build a tokenizer and syntax highlighter for the **Zasm** assembly language project.

---

## Overview

This project implements a custom lexer and syntax highlighter designed specifically for **Zasm**, a lightweight assembly language. Built on top of the **Najd** string-processing framework in C, the utility parses raw assembly code streams, breaks them down into meaningful tokens, and renders them with rich ANSI terminal styling.

---

## Features

* **Macro-Driven Architecture**: Easily scale and define custom token rules using clean macros (`NSR_CONDITION` and `NSR_READER`).
* **Precise Tracking**: Tracks line numbers and character positions dynamically using built-in cursor and buffer abstractions.
* **Contextual Highlighting**: Automatically differentiates regular identifiers from assembly mnemonics (e.g., `ADD`, `LOD`) and jump labels (e.g., `start:`).

---

## Zasm Token Specification

| Token Type | Matching Condition | ANSI Color Output |
| :--- | :--- | :--- |
| **Number** | `isdigit()` | Blue (`\x1b[34m`) |
| **Register** | `R` or `r` followed by digits | Magenta (`\x1b[35m`) |
| **Identifier** | Letters, underscores, or alphanumeric sequences | Yellow / Bold Blue (Mnemonics) / Green (Labels) |
| **Comment** | Begins with `;` until a newline | Dark Gray (`\x1b[90m`) |
| **String** | Enclosed within double quotes (`"..."`) | Green (`\x1b[32m`) |
| **Punctuation** | Characters: `,`, `:`, `+`, `-`, `[`, `]` | White (`\x1b[37m`) |
| **Whitespace** | `isspace()` | None (Transparent) |

---

## Implementation Highlights

1. **The Core Framework (`najd`)**: Manages string boundaries, safe character lookaheads (`najd_string_peek`), state transitions, and automatic memory cleanup for results.
2. **Lookup Tables (`NSR_TABLE`)**: Organizes token priorities and matching routines into a sequential processing pipeline.
3. **Contextual Analysis**: Inspects upcoming tokens in the stream to determine whether an identifier is a code label (identified by a trailing colon `:`) or an instruction mnemonic.