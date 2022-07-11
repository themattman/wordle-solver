#pragma once

#include "common_helpers.h"
#include "wordle_rules.h"
#include "wordle_solver.h"
#include "wordlist_wordle_solver.h"

#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


namespace Helpers {

unique_ptr<WordleSolverImpl> createWordleSolver(const string& solverType, size_t id = 0) {
    unique_ptr<WordleSolverImpl> solverPtr;
    if (solverType == "trie") {
        solverPtr = make_unique<TrieBasedWordleSolver>(id);
    } else if (solverType == "wordlist") {
        solverPtr = make_unique<WordlistWordleSolver>(id);
        // } else {
        //     printUsage();
    }
    return solverPtr;
}

WordleGuess promptUserToCheckWordleGuess(const string& output, size_t guessNumber) {
    cout << "Guess #" << guessNumber << ": " << output << endl;

    string input = collectUserInput(/*isRepeat=*/false, "checking");
    while(!isUserInputValidAnswer(input)) {
        input = collectUserInput(/*isRepeat=*/true, "checking");
    }

    return stringToWordleGuess(output, input);
}

} // namespace Helpers
