# Deployer Tool - Bash Automation Script

An interactive Bash script designed to manage development and production environments, facilitate terminal clearing utilities, test system indicators, and automate secure Git deployment workflows with a built-in safety lock for production environments.

---

## Features

* **Environment Auto-Detection:** Automatically parses configurations from a `.env` file to determine if the active state is `DEVELOPMENT` or `PRODUCTION`.
* **Production Safety Lock:** Prevents accidental pushes to live branches by requiring a strict, manual text confirmation (`YES`) before executing production deployment pipelines.
* **Automated Git Workflow:** Validates the Git tree, stages all changes, and prepares a standardized commit message featuring developer names and execution timestamps.
* **Interactive Terminal UI:** Features custom ASCII banners, styled text layouts, and a simulated environment synchronization progress bar.
* **Advanced Screen Management:** Offers options for standard terminal clears or hard terminal buffer and cache resets.

---

## Requirements

* A system with a Bash environment installed (Linux, macOS, or WSL on Windows).
* Git initialized within the project root directory (`git init`).

---

## Configuration and Setup

1. **Create an Environment File (Optional):** Create a `.env` file in the root directory of the project to customize the application state:
   ```env
   APP_ENV=DEVELOPMENT
   DEFAULT_BRANCH=main