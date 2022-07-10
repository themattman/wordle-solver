#include "game_modes.h"
#include "wordle_buffer.h"
#include "wordle_checker.h"
#include "wordle_helpers.h"
#include "wordle_rules.h"
#include "wordle_selector.h"
#include "wordle_solver.h"
#include "wordlist_wordle_solver.h"

#include <future>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace std;


// Runs one iteration of the Wordle game with automated solver & checker.
bool runOneGame(const string& solverType, const string& answer, size_t idx, shared_ptr<WordleBuffer> wb) {
    auto solver = Helpers::createWordleSolver(solverType);
    auto checker = WordleChecker();
    checker.setAnswer(answer);

    size_t numGuesses = 0;
    auto guess = WordleGuess(solver->makeInitialGuess(wb, idx));
    bool result = checker.check(guess, numGuesses);
    do {
        if (result) {
            if (guess == CorrectWordleGuess) {
                for (size_t i = numGuesses; i < MAX_GUESSES; i++) {
                    wb->write(to_string(1) + ",", idx);
                }
                break;
            }
            solver->processResult(guess);
        }

        guess = WordleGuess(solver->makeSubsequentGuess(numGuesses, wb, idx));
        result = checker.check(guess, numGuesses);
    } while (numGuesses < MAX_GUESSES);

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        wb->write("failure," + to_string(solver->getNumCandidates()) + "," + to_string(numGuesses) + "," + answer, idx, /*newline=*/true);
        return false;
    }

    wb->write("success," + to_string(solver->getNumCandidates()) + "," + to_string(numGuesses) + "," + answer, idx, /*newline=*/true);
    return true;
}

// Runs automated solver across entire dictionary on multiple threads to speed up time to completion.
void runAllWordsMultiThreaded(const string& solverType) {
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    auto wb = make_shared<WordleBuffer>();
    wb->write("guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer", 0, /*newline=*/ true);

    auto threads = vector<future<bool>>();
    for (size_t count = 0; count < words.size(); count++) {
        threads.push_back(async(launch::async, runOneGame, solverType, words[count], count+1, wb));
    }

    for (size_t i = 0; i < threads.size(); i++) {
        if (threads[i].valid()) {
            successes = threads[i].get() ? successes+1 : successes;
        }
    }

    cout << wb->read() << endl;
    cout << successes << "/" << words.size() << "=" << std::setprecision(4) << (static_cast<double>(successes)/static_cast<double>(words.size())) << endl;
    cout << "done." << endl;
}

// Runs automated solver across entire dictionary.
void runAllWords(const string& solverType) {
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    size_t runs = 0;
    auto wb = make_shared<WordleBuffer>();
    wb->write("guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer", 0, /*newline=*/ true);

    for (auto& word : words) {
        if (runOneGame(solverType, word, runs+1, wb)) {
            successes++;
        }
        g_num_runs++;
        runs++;
    }

    cout << wb->read() << endl;
    cout << successes << "/" << runs << "=" << std::setprecision(4) << (static_cast<double>(successes)/static_cast<double>(runs)) << endl;
    cout << "done." << endl;
}

// Run one iteration of solver against `answer`.
// Useful for debugging solver & selector.
void runDebug(unique_ptr<WordleSolverImpl> solver, const string& answer) {
    auto checker = WordleChecker();
    checker.setAnswer(answer);

    auto wb = make_shared<WordleBuffer>();
    size_t numGuesses = 0;
    auto guess = WordleGuess(solver->makeInitialGuess(wb, 1));
    bool result = checker.check(guess, numGuesses);
    if (DEBUG) cout << "> [" << numGuesses << "] " << guess.guessStr << endl;
    do {
        if (result) {
            if (guess == CorrectWordleGuess) {
                break;
            }
            if (numGuesses >= MAX_GUESSES) {
                if (DEBUG) cerr << "[end]result:failure,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver.get())->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
            }
            solver->processResult(guess);
        }
        guess = WordleGuess(solver->makeSubsequentGuess(numGuesses, wb, 1));
        result = checker.check(guess, numGuesses);
        if (DEBUG) cout << "> [" << numGuesses << "] " << guess.guessStr << endl;
        if (result) {
            if (guess == CorrectWordleGuess) {
                break;
            }
            if (numGuesses >= MAX_GUESSES) {
                if (DEBUG) cerr << "[end]result:failure,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver.get())->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
            }
            solver->processResult(guess);
        }
    } while (numGuesses < MAX_GUESSES);

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        if (DEBUG || DEBUG_UNICODE) cerr << "result:failure,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver.get())->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
    } else {
        cout << "\nWordle " << numGuesses << "/" << MAX_GUESSES << endl;
        if (DEBUG) cerr << "result:success,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver.get())->getNumCandidates() << ",num_guesses:" << numGuesses << endl;
    }
}

// Allow user to act as the checker by entering 'B', 'Y', 'G' for each letter.
int interactiveMode(unique_ptr<WordleSolverImpl> solver) {
    auto wb = make_shared<WordleBuffer>();
    size_t numGuesses = 1;
    WordleGuess wg = Helpers::promptUserToCheckGuess(solver->makeInitialGuess(wb, 1), numGuesses);
    if (wg != CorrectWordleGuess) {
        solver->processResult(wg);
        for (numGuesses++; numGuesses <= MAX_GUESSES; numGuesses++) {
            wg = Helpers::promptUserToCheckGuess(solver->makeSubsequentGuess(numGuesses, wb, 1), numGuesses);
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
        cout << "\nWordle " << numGuesses << "/" << MAX_GUESSES << endl;
    }

    return 0;
}

// User provides guesses, then provides hints from the oracle
// Augments IRL game play #sorrynotsorry
int cheatMode(unique_ptr<WordleSolverImpl> solver) {
    // Decls:
    // wordle_trie.cpp:14 WordleTrie::printCandidates(), no scores
    // wordle_selectors.cpp:114 MostCommonLetterSelector<>::printCandidates(), scores
    // Callers:
    // wordlist_wordle_solver.cpp:116 TrieBasedWordleSolver::processResult() -> trie, no scores
    // wordle_selectors.cpp:75 MostCommonLetterSelector<>::select() -> mcl, scores

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
        cout << "\nWordle " << numGuesses << "/" << MAX_GUESSES << endl;
    }

    return 0;
}
