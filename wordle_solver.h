#pragma once

#include "wordle_rules.h"

#include <iostream>
#include <string>

using namespace std;


class WordleSolver {
public:
    WordleSolver() = default;
    virtual string makeInitialGuess() = 0;
    virtual string makeSubsequentGuess() = 0;
    virtual void processResult(const WordleGuess& guess) = 0;
protected:
    vector<WordleKnown> m_knownCorrects{LETTER_COUNT, {char(), WordleResult::BLACK}};
};
