#!/bin/bash
MAZE_DIR=mazes
LOG_FILE=mazes_log.txt

if [[ $# -ne 3 ]] || [[ $1 -lt 0 ]] || [[ $2 -lt 0 ]] || [[ $3 -lt 0 ]]; then
    echo "Usage ./testing_script.sh <height> <width> <test amount>"
    exit 1
fi

if [[ ! -d "$MAZE_DIR" ]]; then
    mkdir $MAZE_DIR
fi

#If an old log file exists delete it, then create an empty log file.
if [[ -f "$LOG_FILE" ]]; then
    rm "$LOG_FILE"
fi
touch "$LOG_FILE"

TOTAL=0

for ((i = 1; i <= $3; i++)); do
    MAZE_PATH="${MAZE_DIR}/maze_${i}.txt"

    python3 ./maze_generator.py $1 $2 "$MAZE_PATH"

    RES="$(./parte1 "$MAZE_PATH")"
    echo "$RES"

    LEN=$(echo "$RES" | wc -m)
    LEN=$((LEN - 1))
    echo "$LEN" >> "$LOG_FILE"

    TOTAL=$((TOTAL + LEN))
done
echo "Average steps to solve a ${1}x${2} maze are: $((TOTAL / $3))"