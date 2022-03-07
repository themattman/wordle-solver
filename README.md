# Wordle Solver

Weekend project to come up with automated strategies to solve Wordle

## Setup
```
g++ -g main.cpp -o solver
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


## Files

- `answers.txt`

  - Primitive 5-letter dictionary from the Internet

- `main.cpp`

  - main() lives here

- `wordle_helpers.h`

  - Plumbing to make user interaction easier

- `wordle_solver.h`

  - Base class. Basic game constraints defined here.

- `wordlist_wordle_solver.h`

  - More intelligent solvers that use a dictionary


## ToDo

- Implement the checker (given the answer, check guesses from user input)

- Get basic strategy working

- More complex strategies with letter distributions from the dictionary

- Better dictionary

- Support changing rules of game to more letters, more guesses, etc

- Select strategy from cmd line

- Yank implementation out of header
