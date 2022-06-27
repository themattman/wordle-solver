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

#include "wordle_buffer.h"
#include "wordle_checker.h"
#include "wordle_helpers.h"
#include "wordle_rules.h"
#include "wordle_selector.h"
#include "wordle_solver.h"
#include "wordlist_wordle_solver.h"

#include <future>
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
    cerr << " -d,--hard   : enable hard mode" << endl;
    cerr << " -m,--mode   : choose between: all,cheat,debug,interactive,one" << endl;
    cerr << " -t,--multi  : multi-threaded (assumes --mode all)" << endl;
    cerr << " -w,--word   : answer word for certain modes" << endl;
    exit(1);
}

std::unique_ptr<WordleSolverImpl> createWordleSolver(const std::string& solverType,
                                                     std::unique_ptr<WordleSelector<SetIterator>>& selector) {
    std::unique_ptr<WordleSolverImpl> solverPtr;
    if (solverType == "trie") {
        solverPtr = make_unique<TrieBasedWordleSolver>(selector);
    } else if (solverType == "wordlist") {
        solverPtr = make_unique<WordlistWordleSolver>(selector);
    } else {
        printUsage();
    }
    return solverPtr;
}

// Runs one iteration of the Wordle game with automated solver & checker.
bool runOneGame(const string& solverType, const string& answer, size_t idx,
                shared_ptr<WordleBuffer> wb, bool isHardMode) {
    auto selector = WordleSelectorFactory<SetIterator>::makeWordleSelector(
        WordleSelectorType::FrequencyAndPositionalLetter); // Choose `WordleSelector` HERE!!
    auto solver = createWordleSolver(solverType, selector);
    auto checker = WordleChecker(isHardMode);
    checker.setAnswer(answer);

    size_t numGuesses = 0;
    auto guess = WordleGuess(solver->makeInitialGuess(wb, idx));
    bool result = checker.check(guess);
    numGuesses++;
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
        result = checker.check(guess);
        numGuesses++;
    } while (numGuesses < MAX_GUESSES);

    if (numGuesses >= MAX_GUESSES && guess != CorrectWordleGuess) {
        wb->write("failure," + to_string(solver->getNumCandidates()) + "," + to_string(numGuesses) + "," + answer, idx, /*newline=*/true);
        return false;
    }

    wb->write("success," + to_string(solver->getNumCandidates()) + "," + to_string(numGuesses) + "," + answer, idx, /*newline=*/true);
    return true;
}

// Runs automated solver across entire dictionary on multiple threads to speed up time to completion.
void runAllWordsMultiThreaded(const string& solverType, bool isHardMode) {
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    auto wb = make_shared<WordleBuffer>();
    wb->write("guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer", 0, /*newline=*/ true);

    auto threads = vector<future<bool>>();
    for (size_t count = 0; count < words.size(); count++) {
        threads.push_back(async(launch::async, runOneGame, solverType, words[count], count+1, wb, isHardMode));
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
void runAllWords(const string& solverType, bool isHardMode) {
    vector<string> words = Helpers::getDictionary();
    size_t successes = 0;
    size_t runs = 0;
    auto wb = make_shared<WordleBuffer>();
    wb->write("guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer", 0, /*newline=*/ true);

    for (auto& word : words) {
        if (runOneGame(solverType, word, runs+1, wb, isHardMode)) {
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
void debugMode(unique_ptr<WordleSolverImpl> solver, const string& answer, bool isHardMode) {
    auto checker = WordleChecker(isHardMode);
    checker.setAnswer(answer);

    auto wb = make_shared<WordleBuffer>();
    size_t numGuesses = 0;
    auto guess = WordleGuess(solver->makeInitialGuess(wb, 1));
    bool result = checker.check(guess);
    numGuesses++;
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
        result = checker.check(guess);
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
        numGuesses++;
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

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("solver,s", po::value<string>(), "choose between: trie(default),wordlist")
        // ("selector,l", po::value<string>(), "EnhancedRandom,FrequencyAndPositionalLetter(default),ImprovedMostCommonLetter,NaiveMostCommonLetter,PositionalLetter,Random")
        ("hard,d", "enable hard mode")
        ("mode,m", po::value<string>(), "choose between: all,cheat,debug,interactive,one")
        ("multi,t", "multi-threaded (assumes --mode all)")
        ("word,w", po::value<string>(), "answer word for certain modes")
        ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(/*argc=*/argc, /*argv=*/argv, /*options_desc=*/desc), vm);
    } catch (const std::exception& e) {
        cout << "Exception Error: " << e.what() << endl;
        return 3;
    }

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
        bool isHardMode = (vm.count("hard")) ? true : false;
        cout << "hard mode:" << boolalpha << isHardMode << endl;
        string solverType = "trie";
        if (vm.count("solver")) {
            solverType = vm["solver"].as<string>();
        }
        auto selector = WordleSelectorFactory<SetIterator>::makeWordleSelector(
            WordleSelectorType::FrequencyAndPositionalLetter); // Choose `WordleSelector` HERE!!
        auto solver = createWordleSolver(solverType, selector);
        auto solverMode = vm["mode"].as<string>();
        cout << "mode:" << solverMode << endl;

        if (solverMode == "all") {
            if (vm.count("multi")) {
                cout << "multi-threaded" << endl << endl;
                runAllWordsMultiThreaded(solverType, isHardMode);
            } else {
                cout << endl;
                runAllWords(solverType, isHardMode);
            }
        } else if (solverMode == "one") {
            if (!vm.count("word")) {
                cerr << "'one' mode requires a 'word' as a solution" << endl;
                printUsage();
            }
            if (!Helpers::isWordInDictionary(vm["word"].as<string>())) {
                cerr << "Error: [" << vm["word"].as<string>() << "] not in the wordlist" << endl;
                return 1;
            }
            cout << endl;
            cout << "TODO: pass unicode in as an option" << endl;
            auto wb = make_shared<WordleBuffer>();
            wb->write("guess1cands,guess2cands,guess3cands,guess4cands,guess5cands,guess6cands,result,words_left,num_guesses,answer", 0, /*newline=*/ true);
            runOneGame(solverType, vm["word"].as<string>(), 1, wb, isHardMode);
        } else if (solverMode == "cheat") {
            cout << endl;
            cheatMode(move(solver));
        } else if (solverMode == "debug") {
            if (!vm.count("word")) {
                cerr << "'debug' mode requires a 'word' as a solution" << endl;
                printUsage();
            }
            if (!Helpers::isWordInDictionary(vm["word"].as<string>())) {
                cerr << "Error: [" << vm["word"].as<string>() << "] not in the wordlist" << endl;
                return 1;
            }
            cout << endl;
            debugMode(move(solver), vm["word"].as<string>(), isHardMode);
        } else if (solverMode == "interactive") {
            cout << endl;
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
