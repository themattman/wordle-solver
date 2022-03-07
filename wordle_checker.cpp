#include "wordle_checker.h"
#include "wordle_rules.h"

#include <fstream>
#include <string>
#include <vector>

bool WordleChecker::check(WordleGuess& wg) {
    if (wg.results.size() > 0) {
        throw;
    }

    if (m_dict.size() == 0) {
        throw;
    }

    if (m_dict.find(wg.guessStr) == m_dict.end()) {
        return false;
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

    return true;
}

bool WordleChecker::setAnswer(string answer) {
    if (answer.size() != LETTER_COUNT) {
        return false;
    }

    m_answer = answer;
    return true;
}

void WordleChecker::loadDictionary(string filename) {
    auto filein = ifstream("answers.txt");
    string word;
    while (std::getline(filein, word))
    {
        if (word.size() == LETTER_COUNT) {
            m_dict.insert(word);
        }
    }
}
