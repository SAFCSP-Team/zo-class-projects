#!/bin/bash

COMMAND=$1
shift # Shift removes the first argument, leaving the rest for the sub-scripts

case "$COMMAND" in
    "img")
        source ./imageso.sh "$@"
        ;;
    "vid")
        source ./videoso.sh "$@"
        ;;
    "aio")
        source ./audioso.sh "$@"
        ;;
    *)
        echo "Usage: micm.sh {img|vid|aio}"
        return 1 2>/dev/null || exit 1
        ;;
esac