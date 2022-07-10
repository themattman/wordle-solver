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

#include "game_modes.h"
#include "wordle_buffer.h"
#include "wordle_checker.h"
#include "wordle_helpers.h"
#include "wordle_rules.h"
#include "wordle_selector.h"
#include "wordle_solver.h"
#include "wordlist_wordle_solver.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string>

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace std;

namespace po = boost::program_options;


void printUsage() {
    cerr << "Usage: solver [-h/--help] --mode mode [-t/--multi] --word word" << endl << endl;
    cerr << "Options:" << endl;
    cerr << " -h,--help   : produce help message" << endl;
    cerr << " -s,--solver : choose between: trie(default),wordlist" << endl;
    cerr << " -m,--mode   : choose between: all,cheat,debug,interactive,one" << endl;
    cerr << " -t,--multi  : multi-threaded (assumes --mode all)" << endl;
    cerr << " -w,--word   : answer word for certain modes" << endl;
    exit(1);
}

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("solver,s", po::value<string>(), "choose between: trie(default),wordlist")
        // ("selector,l", po::value<string>(), "EnhancedRandom,FrequencyAndPositionalLetter(default),ImprovedMostCommonLetter,NaiveMostCommonLetter,PositionalLetter,Random")
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
        string solverType = "trie";
        if (vm.count("solver")) {
            solverType = vm["solver"].as<string>();
        }
        auto solver = Helpers::createWordleSolver(solverType);
        auto solverMode = vm["mode"].as<string>();
        cout << "mode:" << solverMode << endl;

        if (solverMode == "all") {
            if (vm.count("multi")) {
                cout << "multi-threaded" << endl << endl;
                runAllWordsMultiThreaded(solverType);
            } else {
                cout << endl;
                runAllWords(solverType);
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
            runOneGame(solverType, vm["word"].as<string>(), 1, wb);
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
            runDebug(move(solver), vm["word"].as<string>());
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
