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

#include "wordle_checker.h"
#include "wordle_helpers.h"
#include "wordle_rules.h"
#include "wordle_selectors.h"
#include "wordle_solver.h"
#include "wordlist_wordle_solver.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;


// Runs one iteration of the Wordle game with automated solver & checker.
bool runOneGame(const string& answer) {
    auto solver = new TrieBasedWordleSolver();
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
        cerr << "res:failure,words_left:" << solver->getNumCandidates() << ",ng:" << numGuesses << ",word:" << answer << endl;
        return false;
    }

    cerr << "res:success,words_left:" << solver->getNumCandidates() << ",ng:" << numGuesses << endl;
    return true;
}

// Runs automated solver across entire dictionary.
void runAllWords() {
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    size_t runs = 0;
    for (auto& word : words) {
        if (runOneGame(word)) {
            successes++;
        }
        runs++;
        //if (runs == 100) break;
    }
    cout << successes << "/" << runs << "=" << std::setprecision(4) << (static_cast<double>(successes)/static_cast<double>(runs)) << endl;
    cout << "done." << endl;
}

// Run one iteration of solver against `answer`.
// Useful for debugging solver & selector.
void runDebug(const string& answer) {
    auto solver = new TrieBasedWordleSolver();
    auto checker = WordleChecker();
    checker.setAnswer(answer);

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
                if (numGuesses >= MAX_GUESSES) {
                    cerr << "[end]res:failure,words_left:" << solver->getNumCandidates() << ",ng:" << numGuesses << ",word:" << answer << endl;
                }
                solver->processResult(guess);
            }
        }
    }

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        cerr << "res:failure,words_left:" << solver->getNumCandidates() << ",ng:" << numGuesses << ",word:" << answer << endl;
        cout << "Darn!" << endl;
    } else {
        cerr << "res:success,words_left:" << solver->getNumCandidates() << ",ng:" << numGuesses << endl;
        cout << "Hell yeah!" << endl;
        cout << "Wordle " << numGuesses << "/" << MAX_GUESSES << endl;
    }
}

// Allow user to act as the checker by entering 'B', 'Y', 'G' for each letter.
// Could be used to augment user's ability to solve a Wordle IRL.
int interactiveMode() {
    auto solver = TrieBasedWordleSolver();
    size_t numGuesses = 1;
    WordleGuess wg = Helpers::promptUser(solver.makeInitialGuess(), numGuesses);
    if (wg != CorrectWordleGuess) {
        solver.processResult(wg);
        for (numGuesses++; numGuesses <= MAX_GUESSES; numGuesses++) {
            wg = Helpers::promptUser(solver.makeSubsequentGuess(), numGuesses);
            if (wg == CorrectWordleGuess) {
                break;
            }
            solver.processResult(wg);
        }
    }

    if (numGuesses >= MAX_GUESSES && wg != CorrectWordleGuess) {
        cout << "Darn!" << endl;
    } else {
        cout << "Hell yeah!" << endl;
        cout << "Wordle " << numGuesses << "/" << MAX_GUESSES << endl;
    }

    return 0;
}

int main() {
    // Which mode would you like to run?
    // runAllWords();
    // runDebug("haute");
    // interactiveMode();

    return 0;
}
