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
#include "wordle_rules.h"
#include "wordle_selectors.h"

#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;


bool runGame(const string& answer) {
    //auto selector = new RandomSelector();
    try {
        //auto selector = new MostCommonLetterSelector();
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

        //cerr << "res:success,words_left:" << solver->getNumCandidates() << ",ng:" << numGuesses << endl;
        return true;
    } catch (std::runtime_error e) {
        cout << e.what() << endl;
        return false;
    } catch (WordleNoMoreCandidatesException e) {
        cout << e.what() << endl;
        return false;
    } catch (...) {
        cout << "caught generic" << endl;
        return false;
    }
}


void runAllWords() {
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    size_t runs = 0;
    for (auto& word : words) {
        word="write";
        cout << word << endl;
        if (runGame(word)) {
            successes++;
        }
        runs++;
        break;
    }
    cout << successes << "/" << runs << "=" << std::setprecision(4) << (static_cast<double>(successes)/static_cast<double>(runs)) << endl;
    cout << "done." << endl;
}

void runDebug() {
    // auto selector = new RandomSelector();
    auto solver = new TrieBasedWordleSolver();
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
