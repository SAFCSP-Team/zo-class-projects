#!/bin/bash

if [ -z "$BASH_VERSION" ]; then
    echo "❌ Error: This script requires a Bash environment to run!"
    exit 1
fi

RED='\033[1;31m'
GREEN='\033[1;32m'
YELLOW='\033[1;33m'
BLUE='\033[1;34m'
CYAN='\033[1;36m'
MAGENTA='\033[1;35m'
NC='\033[0m'

show_banner() {
    local color=$1
    echo -e "${color}"
    echo "    ____  _____ ____  __    ____  __  ________________"
    echo "   / __ \/ ___// __ \/ /   / __ \/ / / / ____/ ___/  /"
    echo "  / / / /\__ \/ /_/ / /   / / / / /_/ / __/  \__ \   "
    echo " / /_/ /___/ / ____/ /___/ /_/ / __  / /___ ___/ /   "
    echo "/_____//____/_/   /_____/\____/_/ /_/_____//____/    "
}

clean_screen() {
    echo -e "${YELLOW}🧹 Cleaning current screen...${NC}"
    sleep 0.8
    clear
}

delete_screen() {
    echo -e "${RED}💣 Deleting screen and resetting terminal memory...${NC}"
    sleep 0.8
    printf "\033c" 
}

scroll_effect() {
    echo -e "${BLUE}🔄 Loading environment configurations & scripts...${NC}"
    sleep 0.5
    
    for i in {1..20}; do
        let "percent=$i*5"
        bar=""
        for ((j=1; j<=i; j++)); do bar="${bar}█"; done
        for ((j=i; j<20; j++)); do bar="${bar}░"; done
        
        echo -ne "\r${CYAN}[$bar] ${percent}% Completed${NC}"
        sleep 0.08
    done
    echo -e "\n${GREEN}✨ System Environment Synchronized Successfully!${NC}\n"
    sleep 0.8
}

if [ -f .env ]; then
    while IFS= read -r line || [ -n "$line" ]; do
        if [[ ! "$line" =~ ^# && "$line" =~ = ]]; then
            eval "export $line"
        fi
    done < .env
fi

ENV=${APP_ENV:-"DEVELOPMENT"}
BRANCH=${DEFAULT_BRANCH:-"main"}
DEV_NAME="Suhail, Osama, Bandar"

delete_screen

if [ "$ENV" = "PRODUCTION" ]; then
    THEME_COLOR=$RED
    show_banner "$RED"
    echo -e "${RED}┌────────────────────────────────────────────────────────┐${NC}"
    echo -e "${RED}│  ⚠️  CRITICAL WARNING: ACTIVE ENV IS [PRODUCTION] !!!    │${NC}"
    echo -e "${RED}└────────────────────────────────────────────────────────┘${NC}"
else
    THEME_COLOR=$GREEN
    show_banner "$GREEN"
    echo -e "${GREEN}┌────────────────────────────────────────────────────────┐${NC}"
    echo -e "${GREEN}│  🌿 Safe Zone: Active Environment is [DEVELOPMENT]     │${NC}"
    echo -e "${GREEN}└────────────────────────────────────────────────────────┘${NC}"
fi

echo -e "\n${MAGENTA}👤 Active Developers:${NC} ${DEV_NAME}"
echo -e "${MAGENTA}🌿 Target Branch:${NC} ${BRANCH}\n"

echo -e "${CYAN}┌────────────── OPERATIONAL MENU ──────────────┐${NC}"
echo -e "  ${YELLOW}1)${NC} Soft Clear Screen (Standard)"
echo -e "  ${YELLOW}2)${NC} Hard Reset Screen (Full Wipe)"
echo -e "  ${YELLOW}3)${NC} Test Matrix/Scroll Progress Bar"
echo -e "  ${YELLOW}4)${NC} Execute Automated Secure Git Push"
echo -e "  ${YELLOW}5)${NC} Abort & Exit Terminal"
echo -e "${CYAN}└──────────────────────────────────────────────┘${NC}"
read -p "➔ Enter your choice [1-5]: " user_choice

case $user_choice in
    1)
        clean_screen
        show_banner "$THEME_COLOR"
        echo -e "${GREEN}✔ Screen cleared perfectly.${NC}"
        ;;
    2)
        delete_screen
        show_banner "$THEME_COLOR"
        echo -e "${GREEN}✔ Terminal cache and buffer hard reset done.${NC}"
        ;;
    3)
        clear
        show_banner "$THEME_COLOR"
        scroll_effect
        ;;
    4)
        echo -e "\n${BLUE}🔍 Scanning local directory for valid Git Core...${NC}"
        sleep 0.5
        
        if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
            echo -e "${RED}❌ SYSTEM ERROR: Root directory is not initialized with Git!${NC}"
            echo -e "${YELLOW}👉 Run 'git init' to fix this before deploying.${NC}"
            exit 1
        fi

        if [ "$ENV" = "PRODUCTION" ]; then
            echo -e "\n${RED}🛑 PROD DEPLOYMENT DETECTED! 🛑${NC}"
            echo -e "${YELLOW}This will push updates directly to the live server.${NC}"
            read -p "Are you absolutely sure? Type [YES] to bypass lock: " confirm
            if [ "$confirm" != "YES" ]; then
                echo -e "${RED}✖ Deployment rejected. Safety lock engaged!${NC}"
                exit 1
            fi
        fi

        clear
        show_banner "$THEME_COLOR"
        scroll_effect

        git add .
        git commit -m "[$ENV] Automated sync by Deployer tool ($DEV_NAME) on $(date '+%Y-%m-%d %H:%M')"
        
        echo -e "\n${GREEN}🎉  SUCCESS: Changes successfully local-committed!${NC}"
        echo -e "${YELLOW}🚀  Command pipeline is armed and ready to execute: git push origin $BRANCH${NC}"
        ;;
    5)
        echo -e "\n${THEME_COLOR}👋 Execution terminated. Happy coding!${NC}\n"
        exit 0
        ;;
    *)
        echo -e "${RED}❌ Error: Selection sequence unrecognized!${NC}"
        ;;
esac
