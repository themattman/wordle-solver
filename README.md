# Wordle Solver

Weekend project to come up with automated strategies to solve Wordle.

High water mark of my solver: `99.83%` success rate, `3.66696` average guesses.

## Setup
```
$ cmake .
$ time make
[ 10%] Building CXX object CMakeFiles/solver.dir/wordle_selectors.cpp.o
[ 20%] Building CXX object CMakeFiles/solver.dir/wordlist_wordle_solver.cpp.o
[ 30%] Building CXX object CMakeFiles/solver.dir/wordle_trie.cpp.o
[ 40%] Building CXX object CMakeFiles/solver.dir/wordle_checker.cpp.o
[ 50%] Building CXX object CMakeFiles/solver.dir/main.cpp.o
[ 60%] Linking CXX executable solver
[ 60%] Built target solver
[ 70%] Building CXX object CMakeFiles/test_solver.dir/wordle_checker_test.cpp.o
[ 80%] Building CXX object CMakeFiles/test_solver.dir/wordle_checker.cpp.o
[ 90%] Building CXX object CMakeFiles/test_solver.dir/wordle_selectors.cpp.o
[100%] Linking CXX executable test_solver
[100%] Built target test_solver

real    0m9.000s
user    0m8.131s
sys     0m0.674s
```

## How to Use

- Solve Entire Dictionary
```
$ ./solver --mode all
Wordle Solver v4
mode:all

guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer
2315,48,4,3,2,1,success,1,6,aback
2315,6,2,1,1,1,success,1,4,abase
2315,6,3,2,1,1,success,2,4,abate
2315,61,1,1,1,1,success,1,3,abbey
2315,51,4,1,1,1,success,1,4,abbot
2315,136,15,2,1,1,success,1,5,abhor
...
2294/2315=0.9909
done.
```

- Original Interactive Mode
```
$ ./solver --mode interactive
Wordle Solver v4
mode:interactive

Guess #1: sling
> GBGBB
Guess #2: shiny
> GGGBB
Guess #3: shirk
> GGGGG
Success!
Wordle 3/7
```


- Debug Mode (`#define DEBUG_UNICODE=true` for pretty print)
```
$ ./solver --mode debug --word brain
Wordle Solver v4
mode:debug

⬛⬛🟨🟩🟩
⬛⬛⬛🟩🟩
🟩🟩🟩🟩🟩
Wordle 3/6
```

- View Scoring
```
$ sort -hk 2 -t'=' scores.txt > sorted_scores.txt
```

### Test solver against all words in dictionary

- Current usage
```
$ time ./solver --mode all 2>guess_nums_6.csv
Wordle Solver v4
mode:all

2294/2315=0.9909
done.

real    0m18.768s
user    0m17.029s
sys     0m1.063s

$ grep "success" guess_nums_6.csv | cut -d',' -f9 | awk 's+=$1{print s/NR}' | tail -n 1  # Average number of guesses
3.72113
```

- Fastest (Work in Progress)
```
$ time ./solver --mode all --multi 2>guess_nums_6.csv
Wordle Solver v4
mode:all
multi-threaded

done.

real    0m3.050s
user    0m18.391s
sys     0m1.671s

$ grep "success" guess_nums_6.csv | cut -d',' -f9 | awk 's+=$1{print s/NR}' | tail -n 1  # Average number of guesses
X.XXXXX
```

## Best Results

Number of correct games over entire dictionary.

- `v1.0`: 98.14%

  - Average guesses on success: ??

- `v2.0`: 98.57%

  - Average guesses on success: 4.26687

- `v3.0`: 99.83%

  - Average guesses on success: 3.72113

- `v4.0`: 99.09%

  - Average guesses on success: 3.66696

## Overview

There is a `Checker` and `Solver`. These two entities comprise the Wordle game. The `Solver` is the "player" while the `Checker` is the "referee."

Each `Solver` can be mixed and matched with any `Selector`.

Currently the best combo is `TrieBasedWordleSolver` & `PositionalLetterWordleSelector`.

### Solvers

Role: Generate list of candidate words based on dictionary and results from prior guess.

- `WordleSolver`

  - Abstract Base Class. Interface for `Solver`s.

- `WordlistWordleSolver`

  - Abstract Base Class. Adds wordlist functionality.

- `PassthroughWordleSolver`

  - Intended for debugging `Selector`s. Does no processing of results.

- `TrieBaseWordleSolver`

  - Most powerful `Solver`. Uses a letter-based trie data structure to efficiently eliminate potential words based on results of prior guess.

### Selectors

Role: Choose which word in a given list of words should be selected for the current Wordle guess.

- `WordleSelector`

  - Abstract Base Class. Interface for `Selector`s.

- `RandomWordleSelector`

  - From an iterable list of candidate words, chooses candidate randomly.

- `EnhancedRandomWordleSelector`

  - Randomly choose candidate words that contain more than one vowel and do not contain double letters.

- `MostCommonLetterWordleSelector`

  - Abstract Base Class. Interface for `Selector`s that want to compute frequency scores over remaining candidate words.

- `NaiveMostCommonLetterWordleSelector`

  - Basic approach to computing score based on most common letters in all remaining words.

- `ImprovedMostCommonLetterWordleSelector`

  - Improved approach that doesn't score letters that show up multiple times (they're less common) AND letters that are already green (no effect, but it stands to reason).

- `PositionalLetterWordleSelector`

  - Computes which letter is most common in each position. Scores each candidate word based on this and selects best candidate.

- `FrequencyAndPositionalLetterWordleSelector`

  - Blend of overall scoring (frequency) and positional letter scoring

## Files

- `answers.txt`

  - Primitive 5-letter dictionary from the Internet

- `answers_extended.txt`

  - All allowed guesses, larger dictionary than `answers.txt`

- `guess_nums_6.csv`

  - Results from current algorithm, 6 guesses allowed (default)

- `guess_nums_7.csv`

  - Results from current algorithm, 7 guesses allowed

- `main.cpp`

  - `main()` lives here

- `wordle_checker.[h|cpp]`

  - Automated checking against an answer

- `wordle_checker_test.cpp`

  - Tests for the checker

- `wordle_helpers.h`

  - Static plumbing to make user interaction easier

- `wordle_rules.h`

  - Basic game constraints defined here

- `wordle_selectors.[h|cpp]`

  - Selection Algorithms to feed to `Solver`

- `wordle_solver.h`

  - Abstract Base class/interface for the `Solver`

- `wordle_trie.[h|cpp]`

  - Trie data structure used for Wordle

- `wordlist_wordle_solver.[h|cpp]`

  - More intelligent `Solver`s that use a dictionary

## ToDo

- [ ] Better/Additional dictionary - i.e. include allowed guesses dict

- [ ] Select strategy & mode from cmd line

- [X] Measure success rates of various algos against whole dictionary

- [ ] Define richer exceptions and catch them in main to help in debugging

- [ ] Thread each parallel execution (Clang Mac doesn't support threading :( )

- [ ] Break ties with more common letter in dictionary. TieBreaker class???? - mostly an unhit case

- [ ] Add aggressive `easy mode` `Solver` to knock out many letters with words not in remaining set. Likely to lead to 100% accuracy + lower average guesses.

- [X] Implement `hard mode` `Solver`

- [ ] Implement `hard mode` verifier

- [ ] Replace several size checks with NUM_LETTERS to consolidate understanding of what is tied to this global

- [ ] Raw pointers -> smart pointers

- [ ] Track lowest score of letter in word, sort on this somehow in getBestCandidate. Don't pick words with low lowest letter scores even if their score is the highest.

- [ ] How to optimize with easy (non-hard) mode guessing?

- [X] Wordle Cheat Mode, allow user to select a non-top choice and let algo run with that instead of the auto chosen word

- [ ] Make Selectors Composable (scoring, then selection should be done at another layer)

- [ ] Add wordle unicode decoder

- [X] Add arg parsing OR multiple binaries

- [ ] Check guess against guess dict

- [ ] Support one output buffer per game, will help unlock multi-threaded game

  - writing to the buffer is segfaulting, try small example

## Discussion

- [Best Wordle guessing strategy(HN)](https://news.ycombinator.com/item?id=29928263) https://slc.is/archive/best-wordle-strategy-explore-or-exploit/

- [Wordle-Solver - solves 99.7% of common five-letter English words(HN)](https://news.ycombinator.com/item?id=30052677) [(github)](https://github.com/jason-chao/wordle-solver)

- [Wordle-solving state of the art: all optimality results so far(HN)](https://news.ycombinator.com/item?id=30093313) https://www.poirrier.ca/notes/wordle-optimal/

- https://botfights.ai/game/wordle

- https://codereview.stackexchange.com/questions/32817/multithreading-c-loop

- https://unicode-table.com/en/2B1B/

- [Solving Wordle using information theory(YouTube)](https://www.youtube.com/watch?v=v68zYyaEmEA)

## Diagram

```
     WordleSolver
         ^
         |
   WordleSolverImpl
         ^
         |
  WordlistWordleSolver  --------------------------------->  WordleSelector
         ^                                                       ^
         |                                                       |
PassthroughWordleSolver                             PositionalLetterWordleSolver
         ^                                                       ^
         |                                                       |
TrieBasedWordleSolver -----> WordleTrie       FrequencyAndPositionalLetterWordleSelector
```
