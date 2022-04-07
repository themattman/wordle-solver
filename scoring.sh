#!/usr/bin/env bash
# Requires:
#  Set CREATE_SCORES_FILE to true in `wordle_rules.h`
#  $ ./solver &> fails.txt
#  $ sort -hk 2 -t'=' scores.txt > sorted_scores.txt
#  $ ./scoring.sh
#  $ ./scoring.sh | sort -hk 1 -t':'

line_no=0
while read p; do
    if [[ $line_no -eq 0 ]]; then
        line_no=$((line_no+1))
        continue
    fi
    if [[ $p == *"/"* ]]; then
        break
    fi
    sorted_score=$(grep -n "${p}" sorted_scores.txt | cut -d':' -f1)
    score=$(grep -n "${p}" sorted_scores.txt | cut -d':' -f2-)
    printf '%4.4s: %s\n' "${sorted_score}" "${score}"
    #echo "${sorted_score}
    line_no=$((line_no+1))
done <fails.txt
