#include "wordle_checker.h"
#include "wordle_rules.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;


bool WordleChecker::check(WordleGuess& wg) {
    if (m_isHardMode) {
        return checkHardMode(m_wgList);
    }
    return checkEasyMode(m_wgList.back());
    m_numGuessesUnused++;
}

bool WordleChecker::checkEasyMode(WordleGuess& wg) {
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

    resetFrequencyMap();

    auto result = vector<WordleResult>(LETTER_COUNT, WordleResult::BLACK);
    if (DEBUG && !DEBUG_UNICODE) cout << "      ";

    for (size_t i = 0; i < LETTER_COUNT; i++) {
        auto cur_letter = wg.guessStr[i];
        if (cur_letter == m_answer[i] && m_frequencyMap[cur_letter] != 0) {
            m_frequencyMap[cur_letter]--;
            result[i] = WordleResult::GREEN;
        }
    }

    for (size_t i = 0; i < LETTER_COUNT; i++) {
        auto cur_letter = wg.guessStr[i];
        if (cur_letter == m_answer[i]) {
            // frequencyMap already updated
            continue;
        } else if (m_answer.find(cur_letter) != string::npos && m_frequencyMap[cur_letter] != 0) {
            m_frequencyMap[cur_letter]--;
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

    return true;
}

bool WordleChecker::checkHardModeInternalOneRound(const string& curWord, const string& priorWord,
                                                  const vector<WordleResult>& priorResult) {
    // Save frequencies of yellow hints
    unordered_map<char, size_t> letterMap;
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        if (priorResult[i] == WordleResult::YELLOW) {
            if (letterMap.find(priorWord[i]) == letterMap.end()) {
                letterMap[priorWord[i]] = 0;
            }
            letterMap[priorWord[i]]++;
        }
    }

    // Ensure all green and yellow hints followed
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        if (priorResult[i] == WordleResult::GREEN && priorWord[i] != curWord[i]) {
            return false;
        } else if (priorResult[i] == WordleResult::YELLOW) {
            if (letterMap[priorWord[i]] != 0) {
                letterMap[priorWord[i]]--;
            } else {
                return false;
            }
        }
    }

    for (auto it = letterMap.begin(); it != letterMap.end(); it++) {
        if (it->second != 0) {
            return false;
        }
    }

    return true;
}

bool WordleChecker::checkHardModeInternal(const vector<WordleGuess>& wgList) {
    for (size_t i = 1; i < wgList.size(); i++) {
        if (!checkHardModeInternalOneRound(wgList[i].guessStr, wgList[i-1].guessStr, wgList[i-1].results)) {
            return false;
        }
    }

    return true;
}

bool WordleChecker::checkHardMode(vector<WordleGuess>& wg) {
    bool isEasyModeCorrect = checkEasyMode(wg.back());
    bool isHardModeCorrect = false;
    if (isEasyModeCorrect) {
        isHardModeCorrect = checkHardModeInternal(wg);
    }

    return isEasyModeCorrect && isHardModeCorrect;
}

void WordleChecker::resetFrequencyMap() {
    if (!m_frequencyMap.empty()) {
        m_frequencyMap.clear();
        for (size_t i = 0; i < m_answer.size(); i++) {
            auto cur_letter = m_answer[i];
            if (m_frequencyMap.find(cur_letter) == m_frequencyMap.end()) {
                m_frequencyMap[cur_letter] = 0;
            }

            m_frequencyMap[cur_letter]++;
        }
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
