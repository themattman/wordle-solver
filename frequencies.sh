#!/usr/bin/env bash

for x in {a..z}; do
    p1=$(grep -Irs "^$x" answers.txt | wc -l)
    p2=$(grep -Irs "^.$x" answers.txt | wc -l)
    p3=$(grep -Irs "^..$x" answers.txt | wc -l)
    p4=$(grep -Irs "^...$x" answers.txt | wc -l)
    p5=$(grep -Irs "^....$x" answers.txt | wc -l)
    sum_expression="${p1//[[:space:]]/}+${p2//[[:space:]]/}+${p3//[[:space:]]/}+${p4//[[:space:]]/}+${p5//[[:space:]]/}"
    sum=$(echo "${sum_expression}" | bc)
    echo "$x: ${p1},${p2},${p3},${p4},${p5} = $sum"    
done
