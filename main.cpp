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
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace std;

namespace po = boost::program_options;


void printUsage() {
    cerr << "Usage: solver [-h/--help] --mode mode [-t/--multi] --word word" << endl << endl;
    cerr << "Options:" << endl;
    cerr << " -h,--help   : produce help message" << endl;
    cerr << " -s,--solver : choose between: trie(default),wordlist" << endl;
    cerr << " -m,--mode   : choose between: all,debug,interactive,cheat" << endl;
    cerr << " -t,--mult   : multi-threaded (assumes --mode all" << endl;
    cerr << " -w,--word   : answer word for certain modes" << endl;
    exit(1);
}

unique_ptr<WordleSolverImpl> createWordleSolver(const string& solverType) {
    unique_ptr<WordleSolverImpl> solverPtr;
    if (solverType == "trie") {
        solverPtr = make_unique<TrieBasedWordleSolver>();
    } else if (solverType == "wordlist") {
        solverPtr = make_unique<WordlistWordleSolver>();
    } else {
        printUsage();
    }
    return solverPtr;
}



// Runs one iteration of the Wordle game with automated solver & checker.
bool runOneGame(const string& solverType, const string& answer) {
    auto solver = createWordleSolver(solverType);
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
            guess = WordleGuess(solver->makeSubsequentGuess(numGuesses));
            result = checker.check(guess, numGuesses);
            if (result) {
                if (guess == CorrectWordleGuess) {
                    for (size_t i = numGuesses; i < MAX_GUESSES; i++) {
                        cerr << 1 << ",";
                    }
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

    cerr << "success," << solver->getNumCandidates() << "," << numGuesses << "," << answer << endl;
    return true;
}

// Runs automated solver across entire dictionary on multiple threads to speed up time to completion.
void runAllWordsMultiThreaded(const string& solverType) {
    #undef DEBUG
    #define DEBUG false
    #undef PRINT_GUESSES
    #define PRINT_GUESSES false
    vector<string> words = Helpers::getDictionary();
    cerr << "guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer" << endl;
    // TODO: Synchronized cout access of multiple threads

    for (size_t count = 0; count < words.size(); count++) {
        size_t num_threads = std::thread::hardware_concurrency();
        auto threads = vector<thread>();
        for (size_t i = 0; i < num_threads && count < words.size(); i++) {
            threads.emplace_back(thread(&runOneGame, solverType, words[count]));
            count++;
        }

        for (size_t i = 0; i < threads.size(); i++) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }
    }

    cout << "done." << endl;
}

// Runs automated solver across entire dictionary.
void runAllWords(const string& solverType) {
    #undef DEBUG
    #define DEBUG false
    #undef PRINT_GUESSES
    #define PRINT_GUESSES false
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    size_t runs = 0;
    cerr << "guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer" << endl;

    for (auto& word : words) {
        if (runOneGame(solverType, word)) {
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
void runDebug(unique_ptr<WordleSolverImpl> solver, const string& answer) {
    #undef DEBUG
    #define DEBUG true
    #undef DEBUG_UNICODE
    #define DEBUG_UNICODE true
    #undef PRINT_GUESSES
    #define PRINT_GUESSES true
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
                    if (DEBUG) cerr << "[end]result:failure,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver.get())->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
                }
                solver->processResult(guess);
            }
        }
    }

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        if (DEBUG || DEBUG_UNICODE) cerr << "result:failure,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver.get())->getNumCandidates() << ",num_guesses:" << numGuesses << ",answer:" << answer << endl;
    } else {
        cout << "Wordle " << numGuesses << "/" << MAX_GUESSES << endl;
        if (DEBUG) cerr << "result:success,words_left:" << dynamic_cast<TrieBasedWordleSolver*>(solver.get())->getNumCandidates() << ",num_guesses:" << numGuesses << endl;
    }
}

// Allow user to act as the checker by entering 'B', 'Y', 'G' for each letter.
int interactiveMode(unique_ptr<WordleSolverImpl> solver) {
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
int cheatMode(unique_ptr<WordleSolverImpl> solver) {
    #undef DEBUG
    #define DEBUG true
    #undef PRINT_GUESSES
    #define PRINT_GUESSES true
    #undef PRINT_GUESSES_SIZE
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

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("solver,s", po::value<string>(), "choose between: trie(default),wordlist")
        // ("selector,l", po::value<string>(), "EnhancedRandom,FrequencyAndPositionalLetter(default),ImprovedMostCommonLetter,NaiveMostCommonLetter,PositionalLetter,Random")
        ("mode,m", po::value<string>(), "choose between: all,debug,interactive,cheat")
        ("multi,t", "multi-threaded (assumes --mode all")
        ("word,w", po::value<string>(), "answer word for certain modes")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(/*argc=*/argc, /*argv=*/argv, /*options_desc=*/desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        printUsage();
    }

    // TODO: Not supported yet, because this is baked into the solvers.
    //       Need to provide mechanism to pass this through.
    // auto selector =
    // if (vm.count("selector")) {
    // }

    if (vm.count("mode")) {
        cout << "Wordle Solver v" << VERSION << endl;
        string solverType = "trie";
        if (vm.count("solver")) {
            solverType = vm["solver"].as<string>();
        }
        auto solver = createWordleSolver(solverType);
        auto solverMode = vm["mode"].as<string>();
        cout << "mode:" << solverMode << endl << endl;

        if (solverMode == "all") {
            if (vm.count("multi")) {
                runAllWordsMultiThreaded(solverType);
            } else {
                runAllWords(solverType);
            }
        } else if ("cheat") {
            cheatMode(move(solver));
        } else if ("debug") {
            if (!vm.count("word")) {
                cerr << "debug mode requires a 'word' as a solution" << endl;
                printUsage();
            }
            runDebug(move(solver), vm["word"].as<string>());
        } else if ("interactive") {
            interactiveMode(move(solver));
        } else {
            cerr << "mode [" << solverMode << "] not recognized" << endl;
            printUsage();
        }
    } else {
        printUsage();
    }

    return 0;
}
