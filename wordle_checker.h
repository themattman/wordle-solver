#pragma once

#include "wordle_rules.h"

#include <string>

using namespace std;


class WordleChecker {
public:
    WordleChecker() = default;

    // double/triple letters
    void check(WordleGuess& wg) {
        if (wg.results.size() > 0) {
            throw;
        }

        for (size_t i = 0; i < LETTER_COUNT; i++) {
            if (wg.guessStr[i] == m_answer[i]) {
                wg.results.push_back(WordleResult::GREEN);
            } else if (m_answer.find(wg.guessStr[i]) != string::npos) {
                wg.results.push_back(WordleResult::YELLOW);
            } else {
                wg.results.push_back(WordleResult::BLACK);
            }
        }
    }

    bool setAnswer(string answer) {
        if (answer.size() != LETTER_COUNT) {
            return false;
        }

        m_answer = answer;
        return true;
    }
private:
    string m_answer;
};
