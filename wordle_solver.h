#pragma once

#include "wordle_rules.h"
#include "wordle_selectors.h"

#include <string>

using namespace std;


template <typename FwdIter>
class WordleSolver {
public:
    WordleSolver(Selector<FwdIter>* s) { *m_selector = *s; }
    virtual string makeInitialGuess() = 0;
    virtual void processResult(const WordleGuess& guess) = 0;
    virtual string makeSubsequentGuess() = 0;
private:
    Selector<FwdIter>* m_selector;
};
