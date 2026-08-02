#!/bin/bash

export IMG_LOCATION=~/Pictures

system="ubuntu"

check_cmd_status() {
    if [[ $? -eq 0 ]]; then
        echo "Images Moved to $IMG_LOCATION"
    fi
}

if [[ $DESKTOP_SESSION == $system ]]; then
    list_of_files=$(ls)
    
    case "$1" in
        help)
            echo "  o          o   __o__   o              o    o          o  
 <|\        /|>    |    <|>            <|>  <|\        /|> 
 / \\o    o// \   / \   < >            < >  / \\o    o// \ 
 \o/ v\  /v \o/   \o/    \o            o/   \o/ v\  /v \o/ 
  |   <\/>   |     |      v\          /v     |   <\/>   |  
 / \        / \   < >      <\        />     / \        / \ 
 \o/        \o/    |         \o    o/       \o/        \o/ 
  |          |     o          v\  /v         |          |  
 / \        / \  __|>_         <\/>         / \        / \ 
                                                           
                                                           
                                                           "
            echo " Tool usage:
                App [show/move] [empty/type]
            "
            ;;
        show)
            if [[ $2 == "jpg" ]]; then
                find *.jpg
            elif [[ $2 == "png" ]]; then
                find *png
            elif [[ $2 == "jpeg" ]]; then
                find *.jpeg
            else 
                find *.jpg *.png *.jpeg
            fi
            ;;
        move)
            if [[ $2 == "jpg" ]]; then
                mv *.jpg $IMG_LOCATION
                check_cmd_status
            elif [[ $2 == "png" ]]; then
                mv *.png $IMG_LOCATION
                check_cmd_status
            elif [[ $2 == "jpeg" ]]; then
                mv *.jpeg $IMG_LOCATION
                check_cmd_status
            else
                mv *.jpg *.png *.jpeg $IMG_LOCATION
                check_cmd_status
            fi
            ;;
        *)
                echo "usage: [show/move] [empty/type]"
            ;;
    esac
            

fi

