/**
 * Date:    03/06/2022
 * Author:  Matt Kneiser
 * Purpose: Solve Wordle
 */

/*
Where To Edit Strategies:
Solver:   in main()
Selector: in the respective `WordleSolver` constructor. Currently all locations in `wordlist_wordle_solver.cpp`.
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
bool runOneGame(WordleSolver* solver, const string& answer) {
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
        cerr << "failure," << solver->getNumCandidates() << "," << numGuesses << "," << answer << endl;
        return false;
    }

    cerr << "success," << solver->getNumCandidates() << "," << numGuesses << "," << endl;
    return true;
}

// Runs automated solver across entire dictionary.
void runAllWords() {
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    size_t runs = 0;
    cerr << "result,words_left,num_guesses,answer_if_failure" << endl;
    for (auto& word : words) {
        if (runOneGame(word)) {
            successes++;
        }
        runs++;
    }
    cout << successes << "/" << runs << "=" << std::setprecision(4) << (static_cast<double>(successes)/static_cast<double>(runs)) << endl;
    cout << "done." << endl;
}

// Run one iteration of solver against `answer`.
// Useful for debugging solver & selector.
void runDebug(WordleSolver* solver, const string& answer) {
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
                    cerr << "[end]result:failure,words_left:" << solver->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
                }
                solver->processResult(guess);
            }
        }
    }

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        cerr << "result:failure,words_left:" << solver->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
    } else {
        cerr << "result:success,words_left:" << solver->getNumCandidates() << ",num_guesses:" << numGuesses << endl;
        cout << "Wordle " << numGuesses << "/" << MAX_GUESSES << endl;
    }
}

// Allow user to act as the checker by entering 'B', 'Y', 'G' for each letter.
// Could be used to augment user's ability to solve a Wordle IRL.
int interactiveMode(WordleSolver* solver) {
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
        cout << "Failure!" << endl;
    } else {
        cout << "Success!" << endl;
        cout << "Wordle " << numGuesses << "/" << MAX_GUESSES << endl;
    }

    return 0;
}

int main() {
    // Choose `Solver` HERE!!
    auto solver = TrieBasedWordleSolver();

    // Which mode would you like to run?
    // runAllWords(solver);
    // runDebug(solver, "haute");
    interactiveMode(solver);

    return 0;
}
