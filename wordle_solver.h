#pragma once

#include "wordle_rules.h"

#include <string>


class WordleSolver {
public:
    WordleSolver() = default;
    virtual std::string makeInitialGuess() = 0;
    virtual std::string makeSubsequentGuess(size_t numGuess) = 0;
    virtual void processResult(const WordleGuess& guess) = 0;
protected:
    std::vector<WordleKnown> m_knownCorrects{LETTER_COUNT, {char(), WordleResult::BLACK}};
};
