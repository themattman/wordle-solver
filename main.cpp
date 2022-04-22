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
#include <thread>
#include <vector>

using namespace std;


// Runs one iteration of the Wordle game with automated solver & checker.
bool runOneGame(const string& answer) {
    auto solver = TrieBasedWordleSolver();
    auto checker = WordleChecker();
    checker.setAnswer(answer);

    size_t numGuesses = 0;
    auto guess = WordleGuess(solver.makeInitialGuess());
    bool result = checker.check(guess, numGuesses);
    if (guess != CorrectWordleGuess) {
        if (result) {
            solver.processResult(guess);
        }
        while (numGuesses < MAX_GUESSES) {
            guess = WordleGuess(solver.makeSubsequentGuess(numGuesses));
            result = checker.check(guess, numGuesses);
            if (result) {
                if (guess == CorrectWordleGuess) {
                    for (size_t i = numGuesses; i < MAX_GUESSES; i++) {
                        cerr << 1 << ",";
                    }
                    break;
                }
                solver.processResult(guess);
            }
        }
    }

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        cerr << "failure," << solver.getNumCandidates() << "," << numGuesses << "," << answer << endl;
        //cerr << answer << endl;
        return false;
    }

    cerr << "success," << solver.getNumCandidates() << "," << numGuesses << "," << answer << endl;
    return true;
}

// Runs automated solver across entire dictionary on multiple threads to speed up time to completion.
void runAllWordsMultiThreaded() {
    vector<string> words = Helpers::getDictionary();
    cerr << "guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer" << endl;

    for (size_t count = 0; count < words.size(); count++) {
        vector<thread*> threads(std::thread::hardware_concurrency());
        for (size_t i = 0; i <  threads.size(); i++) {
            threads.push_back(new thread(runOneGame, words[count]));
            count++;
        }
        for (size_t i = 0; i < threads.size(); i++) {
            threads[i]->join();
            delete threads[i];
        }
    }

    cout << "done." << endl;
}

// Runs automated solver across entire dictionary.
void runAllWords() {
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    size_t runs = 0;
    cerr << "guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer" << endl;
    //cerr << "result,words_left,num_guesses,answer_if_failure" << endl;
    for (auto& word : words) {
        if (runOneGame(word)) {
            successes++;
        }
        g_num_runs++;
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
    bool result = checker.check(guess, numGuesses);
    if (DEBUG || DEBUG_UNICODE) cout << "> [" << numGuesses << "] " << guess.guessStr << endl;
    if (guess != CorrectWordleGuess) {
        if (result) {
            solver->processResult(guess);
        }
        while (numGuesses < MAX_GUESSES) {
            guess = WordleGuess(solver->makeSubsequentGuess(numGuesses));
            result = checker.check(guess, numGuesses);
            if (DEBUG) cout << "> [" << numGuesses << "] " << guess.guessStr << endl;
            if (result) {
                if (guess == CorrectWordleGuess) {
                    break;
                }
                if (numGuesses >= MAX_GUESSES) {
                    if (DEBUG) cerr << "[end]result:failure,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver)->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
                }
                solver->processResult(guess);
            }
        }
    }

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        if (DEBUG || DEBUG_UNICODE) cerr << "result:failure,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver)->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
    } else {
        cout << "Wordle " << numGuesses << "/" << MAX_GUESSES << endl;
        if (DEBUG) cerr << "result:success,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver)->getNumCandidates() << ",num_guesses:" << numGuesses << endl;
    }
}

// Allow user to act as the checker by entering 'B', 'Y', 'G' for each letter.
int interactiveMode(WordleSolver* solver) {
    size_t numGuesses = 1;
    WordleGuess wg = Helpers::promptUserToCheckGuess(solver->makeInitialGuess(), numGuesses);
    if (wg != CorrectWordleGuess) {
        solver->processResult(wg);
        for (numGuesses++; numGuesses <= MAX_GUESSES; numGuesses++) {
            wg = Helpers::promptUserToCheckGuess(solver->makeSubsequentGuess(numGuesses), numGuesses);
            if (wg == CorrectWordleGuess) {
                break;
            }
            solver->processResult(wg);
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

// User provides guesses, then provides hints from the oracle
// Augments IRL game play #sorrynotsorry
int cheatMode(WordleSolver* solver) {
    #define DEBUG true
    #define PRINT_GUESSES true
    #define PRINT_GUESSES_SIZE 10

    size_t numGuesses = 1;
    string userGuess = Helpers::promptUserToMakeGuess(numGuesses);
    WordleGuess wg = Helpers::promptUserToCheckGuess(userGuess, numGuesses);
    if (wg != CorrectWordleGuess) {
        solver->processResult(wg);
        for (numGuesses++; numGuesses <= MAX_GUESSES; numGuesses++) {
            userGuess = Helpers::promptUserToMakeGuess(numGuesses);
            wg = Helpers::promptUserToCheckGuess(userGuess, numGuesses);
            if (wg == CorrectWordleGuess) {
                break;
            }
            solver->processResult(wg);
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
    auto solver = new TrieBasedWordleSolver();

    // Which mode would you like to run?
    // runAllWords();
    //runDebug(solver, "jewel");
    //runDebug(solver, "shave");

    // runDebug(solver, "catch");
    // cout << "Remaining Cand" << endl;

    // solver = new TrieBasedWordleSolver();
    // runDebug(solver, "graze");

    // solver = new TrieBasedWordleSolver();
    // cout << "Remaining Cand" << endl;
    // runDebug(solver, "hound");

    // solver = new TrieBasedWordleSolver();
    // cout << "Remaining Cand" << endl;
    // runDebug(solver, "taunt");

    //interactiveMode(solver);
    cheatMode(solver);

    return 0;
}
