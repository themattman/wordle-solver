#pragma once

#include <string>
#include <vector>

static const size_t MAX_GUESSES  = 6;
static const size_t LETTER_COUNT = 5;

enum class WordleResult {
    GREEN,
    YELLOW,
    BLACK
};

struct WordleGuess {
    WordleGuess(std::string cGuess, std::vector<WordleResult> cResults)
        : guessStr(cGuess), results(cResults) {}
    WordleGuess(std::string cGuess)
        : guessStr(cGuess) {
            auto results = std::vector<WordleResult>{};
        }
    std::string guessStr;
    std::vector<WordleResult> results;
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
