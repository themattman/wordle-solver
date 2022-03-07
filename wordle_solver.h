#pragma once

#include "wordle_rules.h"
#include <string>

using namespace std;


class WordleSolver {
public:
    WordleSolver() {}
    virtual string makeInitialGuess() = 0;
    virtual void processResult(const WordleGuess& guess) = 0;
    virtual string makeSubsequentGuess() = 0;
};
