#include "wordle_checker.h"
#include "wordle_rules.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;


bool WordleChecker::check(WordleGuess& wg, size_t& outNumGuesses) {
    if (m_answer.size() != LETTER_COUNT) {
        if (DEBUG || DEBUG_UNICODE) cerr << "Error: [checker] answer not correct size" << endl;
        throw;
    }

    if (wg.results.size() > 0) {
        if (DEBUG || DEBUG_UNICODE) cerr << "Error: [checker] no results to check" << endl;
        throw;
    }

    if (m_dict.size() == 0) {
        if (DEBUG || DEBUG_UNICODE) cerr << "Error: [checker] empty dictionary" << endl;
        throw;
    }

    if (m_dict.find(wg.guessStr) == m_dict.end()) {
        if (DEBUG || DEBUG_UNICODE) cerr << "Not in dictionary. Try again." << endl;
        return false;
    }

    auto result = vector<WordleResult>(LETTER_COUNT, WordleResult::BLACK);
    resetFrequencyMap();
    if (DEBUG && !DEBUG_UNICODE) cout << "      ";

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
            if (DEBUG && !DEBUG_UNICODE) cout << "G";
            if (DEBUG_UNICODE) cout << "\360\237\237\251";
            break;
        case WordleResult::YELLOW:
            wg.results.push_back(WordleResult::YELLOW);
            if (DEBUG && !DEBUG_UNICODE) cout << "Y";
            if (DEBUG_UNICODE) cout << "\360\237\237\250";
            break;
        case WordleResult::BLACK:
            wg.results.push_back(WordleResult::BLACK);
            if (DEBUG && !DEBUG_UNICODE) cout << "B";
            if (DEBUG_UNICODE) {
                if (LIGHT_MODE) cout << "\342\254\234";
                else cout << "\342\254\233";
            }
            break;
        }
    }

    if (DEBUG || DEBUG_UNICODE) cout << endl;

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
    if (DEBUG) cerr << "Answer set to: [" << answer << "]" << endl;

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
    auto filein = ifstream(filename);
    string word;
    while (std::getline(filein, word))
    {
        if (word.size() == LETTER_COUNT) {
            m_dict.insert(word);
        }
    }
}
