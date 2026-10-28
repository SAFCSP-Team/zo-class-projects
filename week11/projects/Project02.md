# Shell Script Project
A complete shell script that does one specific job and is easy for anyone to use.

## Overview
Each student picks a task and builds a **comprehensive** shell script that serves that **specific** role, such as backing up files, cleaning up a folder, or checking system status. The script should be easy to use by **anyone**, with clear options, a help message, and helpful errors.

## Example
A backup script (one possible idea)

Command
```
./backup.sh -s ~/projects -d ~/backups
```
Output
```
Backing up /home/user/projects ...
Backup saved: /home/user/backups/projects_2026-10-28.tar.gz (12 MB)
```
Help
```
./backup.sh -h
```
```
Usage: backup.sh -s <source> -d <destination>
  -s   folder to back up
  -d   folder to save the backup in
  -h   show this help
```

## How It Works
- The user runs the script with options instead of editing the script itself
- The script checks its input first (missing options, folders that don't exist) and prints a clear error if something is wrong
- It performs its task and prints what it did
- `-h` shows how to use the script
- It exits with `0` on success and a non-zero code on failure
