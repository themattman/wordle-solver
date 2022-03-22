# Wordle Solver

Weekend project to come up with automated strategies to solve Wordle

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

- Current Mode
```
$ ./solver
result,words_left,num_guesses,answer_if_failure
success,1,5,
success,1,6,
success,1,5,
success,1,4,
success,1,5,
...
failure,1,6,baker
...
success,6,4,
2294/2315=0.9909
done.
```

- Original Interactive Mode (see comments in `main.cpp` to enable)
```
$ ./solver
Guess #1: sling
> GBGBB
Guess #2: shiny
> GGGBB
Guess #3: shirk
> GGGGG
Success!
Wordle 3/7
```

### Test solver against all words in dictionary

- Current usage
```
$ time ./solver 2>guess_nums_6.csv
2282/2315=0.9857
done.

real    0m18.768s
user    0m17.029s
sys     0m1.063s

$ grep "success" guess_nums_6.csv | cut -d',' -f3 | cut -d':' -f2 | awk 's+=$1{print s/NR}' | tail -n 1  # Average number of guesses
4.26687
```

## Best Results

Number of correct games over entire dictionary.

- `v1.0`: 98.14%

  - Average guesses on success: ??

- `v2.0`: 98.57%

  - Average guesses on success: 4.26687

## Overview

There is a `Checker` and `Solver`. These two entities comprise the Wordle game. The `Solver` is the "player" while the `Checker` is the "referee."

Each `Solver` can be mixed and matched with any `Selector`.

Currently the best combo is `TrieBasedWordleSolver` & `PositionalLetterSelector`.

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

- `Selector`

  - Abstract Base Class. Interface for `Selector`s.

- `RandomSelector`

  - From an iterable list of candidate words, chooses candidate randomly.

- `EnhancedRandomSelector`

  - Randomly choose candidate words that contain more than one vowel and do not contain double letters.

- `MostCommonLetterSelector`

  - Abstract Base Class. Interface for `Selector`s that want to compute frequency scores over remaining candidate words.

- `NaiveMostCommonLetterSelector`

  - Basic approach to computing score based on most common letters in all remaining words.

- `ImprovedMostCommonLetterSelector`

  - Improved approach that doesn't score letters that show up multiple times (they're less common) AND letters that are already green (no effect, but it stands to reason).

- `PositionalLetterSelector`

  - Computes which letter is most common in each position. Scores each candidate word based on this and selects best candidate.


## Files

- `answers.txt`

  - Primitive 5-letter dictionary from the Internet

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

- [ ] Better/Additional dictionary

- [ ] Select strategy & mode from cmd line

- [ ] Measure success rates of various algos against whole dictionary

- [ ] Define richer exceptions and catch them in main to help in debugging

- [ ] Thread each parallel execution (Clang Mac doesn't support threading :( )

- [ ] Break ties with more common letter in dictionary. TieBreaker class???? - mostly an unhit case

- [ ] Add aggressive `easy mode` `Solver` to knock out many letters with words not in remaining set. Likely to lead to 100% accuracy + lower average guesses.

- [ ] Implement `hard mode` `Solver`

- [ ] Replaces several size checks with NUM_LETTERS to consolidate understanding of what is tied to this global

## Discussion

- [Best Wordle guessing strategy](https://news.ycombinator.com/item?id=29928263) https://slc.is/archive/best-wordle-strategy-explore-or-exploit/

- [Wordle-Solver - solves 99.7% of common five-letter English words](https://news.ycombinator.com/item?id=30052677) [(github)](https://github.com/jason-chao/wordle-solver)

- [Wordle-solving state of the art: all optimality results so far](https://news.ycombinator.com/item?id=30093313) https://www.poirrier.ca/notes/wordle-optimal/
