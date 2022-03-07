#pragma once

#include <vector>

static const size_t MAX_GUESSES  = 6;
static const size_t LETTER_COUNT = 5;

enum class WordleResult {
    GREEN,
    YELLOW,
    BLACK
};

auto CorrectWordleGuess = WordleGuess("", {WordleResult::GREEN,
                                           WordleResult::GREEN,
                                           WordleResult::GREEN,
                                           WordleResult::GREEN,
                                           WordleResult::GREEN});

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
