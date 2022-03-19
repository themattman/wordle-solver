#include "wordle_checker.h"
#include "wordle_rules.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

bool WordleChecker::check(WordleGuess& wg, size_t& outNumGuesses) {
    if (m_answer.size() != LETTER_COUNT) {
        throw;
    }

    if (wg.results.size() > 0) {
        throw;
    }

    if (m_dict.size() == 0) {
        throw;
    }

    if (m_dict.find(wg.guessStr) == m_dict.end()) {
        cout << "Not in dictionary. Try again." << endl;
        return false;
    }

    auto result = vector<WordleResult>(LETTER_COUNT, WordleResult::BLACK);

    resetFrequencyMap();
    cout << "      ";
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        if (wg.guessStr[i] == m_answer[i] && m_frequencyMap[wg.guessStr[i]] != 0) {
            m_frequencyMap[wg.guessStr[i]]--;
            result[i] = WordleResult::GREEN;
        }
    }

    for (size_t i = 0; i < LETTER_COUNT; i++) {
        if (wg.guessStr[i] == m_answer[i]) {
            // frequencyMap already updated
            continue;
        } else if (m_answer.find(wg.guessStr[i]) != string::npos && m_frequencyMap[wg.guessStr[i]] != 0) {
            m_frequencyMap[wg.guessStr[i]]--;
            result[i] = WordleResult::YELLOW;
        } else {
            result[i] = WordleResult::BLACK;
        }
    }

    for (auto& r : result) {
        switch(r) {
        case WordleResult::GREEN:
            wg.results.push_back(WordleResult::GREEN);
            cout << "G";
            break;
        case WordleResult::YELLOW:
            wg.results.push_back(WordleResult::YELLOW);
            cout << "Y";
            break;
        case WordleResult::BLACK:
            wg.results.push_back(WordleResult::BLACK);
            cout << "B";
            break;
        }
    }
    cout << endl;

    outNumGuesses = ++m_numGuesses;
    return true;
}

void WordleChecker::resetFrequencyMap() {
    m_frequencyMap.clear();
    for (size_t i = 0; i < m_answer.size(); i++) {
        if (m_frequencyMap.find(m_answer[i]) == m_frequencyMap.end()) {
            m_frequencyMap[m_answer[i]] = 0;
        }
        m_frequencyMap[m_answer[i]]++;
    }
}

bool WordleChecker::setAnswer(string answer) {
    if (answer.size() != LETTER_COUNT) {
        return false;
    }

    m_answer = answer;
    cerr << "Answer set to: [" << answer << "]" << endl;
    return true;
}

void WordleChecker::setRandomAnswer() {
    srand (time(NULL));
    size_t offset = rand() % m_dict.size();
    auto it = m_dict.begin();
    advance(it, offset);
    setAnswer(*it);
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
