#pragma once

#include "wordle_rules.h"
#include "wordle_selectors.h"

#include <iostream>
#include <string>

using namespace std;


class WordleSolver {
public:
    WordleSolver() = default;
    WordleSolver(Selector* s) { cout << "WS" << endl; m_selector = s; cout << "WS:" << s << "|" << m_selector << endl; }
    virtual string makeInitialGuess() = 0;
    virtual string makeSubsequentGuess() = 0;
    virtual void processResult(const WordleGuess& guess) = 0;
protected:
    Selector* m_selector;
};
