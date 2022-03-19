# Wordle Solver

Weekend project to come up with automated strategies to solve Wordle

## Setup
```
cmake .
make
```

## How to Use

```
$ ./solver
Size of wordlist: 2315
candidateWord not accepted:aback
candidateWord accepted:rainy
Guess #1: reach
> BBBYG # user input
Guess #2: ponds
> BYBBB # user input
Guess #3: cloth
> GGGGG # user input
Hell yeah!
Wordle 3/6
```

### Test solver against all words in dictionary
```
$ ./solver 2>guess_nums.txt
$ cut -d':' -f2 guess_nums.txt | awk 's+=$1{print s/NR}' | tail -n 1   # Average number of guesses
```

## Overview

There is a Checker & a Solver. These two entities comprise the Wordle game. The Solver is the "player" while

the Checker is the "referee."


## Files

- `answers.txt`

  - Primitive 5-letter dictionary from the Internet

- `main.cpp`

  - main() lives here

- `wordle_checker.[h|cpp]`

  - Automated checking against an answer

- `wordle_checker_test.cpp`

  - Tests for the checker

- `wordle_helpers.h`

  - Static plumbing to make user interaction easier

- `wordle_rules.h`

  - Basic game constraints defined here

- `wordle_selectors.[h|cpp]`

  - Selection Algorithms to feed to Solver

- `wordle_solver.h`

  - Base class/interface for the Solver

- `wordle_trie.[h|cpp]`

  - Trie data structure used for Wordle

- `wordlist_wordle_solver.[h|cpp]`

  - More intelligent solvers that use a dictionary


## ToDo

- Get basic strategy working

- More complex strategies with letter distributions from the dictionary

- Better dictionary

- Support changing rules of game to more letters, more guesses, etc

- Select strategy from cmd line

- Automate hundreds of rounds of Solver v Checker

- Measure success rates of various algos against whole dictionary
