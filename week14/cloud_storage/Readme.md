# Cloud Storage — TCP Client/Server (C)

A simple TCP-based cloud storage system written in C. The client connects to a server over a socket and can list, upload, and download files.

## Features

- **List** — request the list of files currently stored on the server
- **Upload** — send a local file to the server
- **Download** — retrieve a file from the server and save it locally
- **Bye** — gracefully close the connection

## Build

```bash
gcc -o client client.c
gcc -o server server.c
```

> Replace `client.c` / `server.c` with your actual source file names.

## Usage

Start the server first (on the machine that will host the files):

```bash
./server <portno>
```

Then run the client, pointing it at the server's IP and port:

```bash
./client <Server_IP> <portno>
```

### Client menu

```
--- Cloud Storage Menu ---
1. list
2. upload <filename>
3. download <filename>
4. bye
```

- `list` — prints the files available on the server
- `upload <filename>` — uploads `<filename>` from the current directory to the server
- `download <filename>` — downloads `<filename>` from the server into the current directory
- `bye` — sends an exit signal and closes the connection

## Protocol Overview

Communication is a simple raw-command protocol over TCP:

| Command    | Client → Server                          | Server → Client                     |
|------------|-------------------------------------------|--------------------------------------|
| `LIST`     | `"LIST"` (4 bytes)                        | Newline-separated file list as text (or `"No files found on server.\n"`) |
| `UPLOAD`   | `"UPLOAD"` + filename (256B) + size (long) + file bytes | `"Upload completed successfully"` |
| `DOWNLOAD` | `"DOWNLOAD"` + filename (256B)            | File size (long) + file bytes, or `-1` (long) if not found |
| `EXIT`     | `"EXIT"` (4 bytes)                        | connection closed                    |

Files are stored server-side under `./cloud_storage/` (created automatically on startup if it doesn't exist).

## Server Design Notes

- **Single client at a time.** The server calls `accept()` once, then serves that one connection in a loop until `EXIT` or disconnect. It does not `fork()`/thread per client or loop back to `accept()` for a new connection — a second client can't connect until the server process is restarted.
- Only regular files (`DT_REG`) are included in the `LIST` output; subdirectories inside `cloud_storage/` are skipped.
- Uploaded/downloaded filenames are used as-is to build the server-side path (`STORAGE_DIR + filename`) — no path sanitization, so a filename containing `../` could escape the storage directory.

## Known Limitations / Next Steps

This is a course project and has a few rough edges to be aware of (and good candidates for improvement or discussion in a defense/presentation):

- Uses `usleep()` between writes instead of proper message framing on both client and server — should be replaced with a fixed-size header (opcode + length-prefixed fields).
- `read()`/`write()` calls assume full transfer in one call; should use `readn()`/`writen()` wrapper functions to handle short reads/writes.
- Return value of `open()` is not checked on the server's upload path (a failed `open()` silently drops the incoming file).
- Server only handles one client connection for its entire lifetime.
- No filename sanitization — potential path traversal via `../` in filenames.
- No authentication or encryption — not intended for use over untrusted networks.

## Project Structure

```
.
├── client.c        # TCP client (menu-driven)
├── server.c         # TCP server (handles list/upload/download)
├── cloud_storage/    # created at runtime; stores uploaded files
└── README.md
```

## Requirements

- POSIX-compliant system (Linux/macOS)
- GCC or any C compiler with standard socket headers (`sys/socket.h`, `netinet/in.h`, `netdb.h`)