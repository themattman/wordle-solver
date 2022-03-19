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


bool runGame(const string& answer) {
    auto selector = new RandomSelector();
    auto solver = new TrieBasedWordleSolver(selector);
    auto checker = WordleChecker();
    checker.setAnswer(answer);

    size_t numGuesses = 0;
    auto guess = WordleGuess(solver->makeInitialGuess());
    bool result = checker.check(guess, numGuesses);
    if (guess != CorrectWordleGuess) {
        if (result) {
            solver->processResult(guess);
        }
        while (numGuesses < MAX_GUESSES) {
            guess = WordleGuess(solver->makeSubsequentGuess());
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
        return false;
    }

    cerr << "ng:" << numGuesses << endl;
    return true;
}


void runAllWords() {
    size_t sz = Helpers::getSizeOfDictionary();
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    size_t runs = 0;
    for (; runs < sz; runs++) {
        if (runGame(words[runs])) {
            successes++;
            cout << "success" << endl;
        } else {
            cout << "failure" << endl;
        }
    }
    cerr << successes << "/" << runs << endl;
    cout << successes << "/" << runs << endl;
}

void runDebug() {
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
}

int main() {
    runAllWords();
    // runDebug();

    return 0;
}
