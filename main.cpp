/**
 * Date:    03/06/2022
 * Author:  Matt Kneiser
 * Purpose: Solve Wordle
 */

/*
$ ./solver [strategy selection]
1. Echo best guess
2. Checker checks the guess
<repeat MAX_GUESSES times>
 */

#include "wordle_solver.h"
#include "wordlist_wordle_solver.h"
#include "wordle_checker.h"
#include "wordle_helpers.h"
#include "wordle_selectors.h"


#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;


int main() {
    auto selector = new RandomSelector();
    auto solver = new TrieBasedWordleSolver(selector);
    auto checker = WordleChecker();
    checker.setAnswer("haute");

    size_t numGuesses = 0;
    auto guess = WordleGuess(solver->makeInitialGuess());
    cout << "> [" << numGuesses << "] " << guess.guessStr << endl;
    bool result = checker.check(guess, numGuesses);
    if (guess != CorrectWordleGuess) {
        if (result) {
            solver->processResult(guess);
        }
        while (numGuesses < MAX_GUESSES) {
            guess = WordleGuess(solver->makeSubsequentGuess());
            cout << "> [" << numGuesses << "] " << guess.guessStr << endl;
            result = checker.check(guess, numGuesses);
            if (result) {
                if (guess == CorrectWordleGuess) {
                    break;
                }
                solver->processResult(guess);
            }
        }
    }

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        cout << "Darn!" << endl;
    } else {
        cout << "Hell yeah!" << endl;
        cout << "Wordle " << numGuesses << "/" << MAX_GUESSES << endl;
    }

    return 0;
}
