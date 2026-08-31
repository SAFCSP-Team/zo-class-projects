#!/bin/bash
color_red="\033[0;31m"
color_green="\033[0;32m"
color_yellow="\033[0;33m"
color_blue="\033[0;34m"
color_magenta="\033[0;35m"
color_cyan="\033[0;36m"
color_reset="\033[0m"
color_bold="\033[1m"



echo "┌──────────────────────────────────────────────────────────┐
│     ____      ____            _       ____               │
│    / ___|    |  _ \ _ __ ___ (_)     / ___| ___ _ __     │
│   | |        | |_) | '__/ _ \| |    | |  _ / _ \ '_ \    │
│   | |___     |  __/| | | (_) | |    | |_| |  __/ | | |   │
│    \____|    |_|   |_|  \___// |     \____|\___|_| |_|   │
│                            |__/                          │
└──────────────────────────────────────────────────────────┘"


#  gen project folder

echo -e " ${color_bold} ${color_cyan}C Project Generator${color_reset}"
read -p "Enter project name: " project

while [[ -z "$project" ]]; do
    echo -e "${color_red}Project name can't be empty${color_reset}"
    read -p "Project name: " project
done



echo -e "${color_yellow}Creating project $project...${color_reset}"
mkdir -p "$project/src" "$project/include" "$project/bin"

echo ""


touch  "$project/src/main.c"

cat >"$project/src/main.c" <<'EOF'
#include <stdio.h>

int main() {
printf("Hello, World!\n");
return 0;
}
EOF

echo -e "${color_green}Project $project created src/main.c${color_reset}"

read -p "files to include in the project (separated by space): " -a files


# gen source files

for file in "${files[@]}"; do

# .c
  cat > "$project/src/$file.c" << EOF
  #include "../include/$file.h"

EOF

# .h

 cat > "$project/include/$file.h" << EOF
  #ifndef __${file^^}_H__
  #define __${file^^}_H__

  #endif

EOF

  echo -e "${color_green}Created src/$file.c${color_reset}"

done


# Gen readme
cat >"$project/README.md" << EOF
# $project

## Compilation

\`\`\`bash
gcc src/*.c -o bin/$project
\`\`\`

## Run

\`\`\`bash
./bin/$project
\`\`\`

EOF


cd ./$project
git init
git add .
git commit -m "init"