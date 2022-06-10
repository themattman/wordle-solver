#pragma once

#include "wordle_buffer.h"
#include "wordle_rules.h"

#include <string>

using buf_ptr = std::shared_ptr<WordleBuffer>;

class WordleSolver {
public:
    WordleSolver(std::unique_ptr<WordleSelector<SetIterator>> selector) : m_selector(move(selector)) {}
    virtual ~WordleSolver() {}
    virtual std::string makeInitialGuess(buf_ptr wb, size_t idx) = 0;
    virtual std::string makeSubsequentGuess(size_t numGuess, buf_ptr wb, size_t idx) = 0;
    virtual void processResult(const WordleGuess& guess) = 0;
    virtual size_t getNumCandidates() const = 0;
protected:
    void getInitialRange();
    void getSubsequentRange();

    std::unique_ptr<WordleSelector<SetIterator>> m_selector;
    std::vector<WordleKnown> m_knownCorrects{LETTER_COUNT, {char(), WordleResult::BLACK}};
};

class WordleSolverImpl : public WordleSolver {
public:
    ~WordleSolverImpl() {}
    std::string makeInitialGuess(buf_ptr wb, size_t idx) {
        getInitialRange(iterBegin, iterEnd);
        return "guess";
    }
    std::string makeSubsequentGuess(size_t numGuess, buf_ptr wb, size_t idx) {
        getSubsequentRange(SetIterator begin, SetIterator end);
        return "guess";
    }
    void processResult(const WordleGuess& guess) {}
    size_t getNumCandidates() const { return 0; }
protected:
    std::vector<WordleKnown> m_knownCorrects{LETTER_COUNT, {char(), WordleResult::BLACK}};
};
