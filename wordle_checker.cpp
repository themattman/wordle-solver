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

    cout << "      ";
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        if (wg.guessStr[i] == m_answer[i]) {
            wg.results.push_back(WordleResult::GREEN);
            cout << "G";
        } else if (m_answer.find(wg.guessStr[i]) != string::npos) {
            wg.results.push_back(WordleResult::YELLOW);
            cout << "Y";
        } else {
            wg.results.push_back(WordleResult::BLACK);
            cout << "B";
        }
    }

    cout << endl;
    outNumGuesses = ++m_numGuesses;
    return true;
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
