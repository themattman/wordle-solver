#pragma once

#include <string>
#include <vector>

#define DEBUG false
#define DEBUG_UNICODE true
#define PRINT_GUESSES true
#define LIGHT_MODE false
#define CREATE_SCORES_FILE false
#define DICTIONARY_FILENAME "answers.txt"
#define DICTIONARY_SCORES_FILENAME "scores.txt"

static size_t g_num_runs = 0;
static const size_t MAX_GUESSES  = 6;
static const size_t LETTER_COUNT = 5;

enum class WordleResult {
    GREEN,
    YELLOW,
    BLACK
};

struct WordleKnown {
    char letter;
    WordleResult result;
};

struct WordleGuess {
    WordleGuess(std::string cGuess, std::vector<WordleResult> cResults)
        : guessStr(cGuess), results(cResults) {}
    WordleGuess(std::string cGuess)
        : guessStr(cGuess) {
            auto results = std::vector<WordleResult>{};
    }
    inline bool operator==(const WordleGuess& w1) const {
        return w1.results == results;
    }
    inline bool operator!=(const WordleGuess& w1) const {
        return w1.results != results;
    }

    std::string guessStr;
    std::vector<WordleResult> results;
};

static auto CorrectWordleGuess = WordleGuess("", {WordleResult::GREEN,
                                                  WordleResult::GREEN,
                                                  WordleResult::GREEN,
                                                  WordleResult::GREEN,
                                                  WordleResult::GREEN});
