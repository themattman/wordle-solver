#pragma once

#include "wordle_rules.h"

#include <string>


class WordleSolver {
public:
    WordleSolver() = default;
    virtual ~WordleSolver() {}
    virtual std::string makeInitialGuess() = 0;
    virtual std::string makeSubsequentGuess(size_t numGuess) = 0;
    virtual void processResult(const WordleGuess& guess) = 0;
    virtual size_t getNumCandidates() const = 0;
protected:
    std::vector<WordleKnown> m_knownCorrects{LETTER_COUNT, {char(), WordleResult::BLACK}};
};

class WordleSolverImpl : public WordleSolver {
public:
    WordleSolverImpl() = default;
    ~WordleSolverImpl() {}
    std::string makeInitialGuess() { return "guess"; }
    std::string makeSubsequentGuess(size_t numGuess) { return "guess"; }
    void processResult(const WordleGuess& guess) {}
    size_t getNumCandidates() const { return 0; }
protected:
    std::vector<WordleKnown> m_knownCorrects{LETTER_COUNT, {char(), WordleResult::BLACK}};
};
