#pragma once

#include <string>
#include <vector>

using namespace std;

static const size_t MAX_GUESSES  = 6;
static const size_t LETTER_COUNT = 5;

enum class WordleResult {
    GREEN,
    YELLOW,
    BLACK
};

struct WordleGuess {
    WordleGuess(string cGuess, vector<WordleResult> cResults)
        : guessStr(cGuess), results(cResults) {}
    string guessStr;
    vector<WordleResult> results;
    inline bool operator==(const WordleGuess& w1) const {
        return w1.results == results;
    }
    inline bool operator!=(const WordleGuess& w1) const {
        return w1.results != results;
    }
};

auto CorrectWordleGuess = WordleGuess("", {WordleResult::GREEN,
                                           WordleResult::GREEN,
                                           WordleResult::GREEN,
                                           WordleResult::GREEN,
                                           WordleResult::GREEN});

class WordleSolver {
public:
    WordleSolver() {}
    virtual string makeInitialGuess() = 0;
    virtual void processResult(const WordleGuess& guess) = 0;
    virtual string makeSubsequentGuess() = 0;
};
