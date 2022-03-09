#pragma once

#include "wordle_rules.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;


class WordleChecker {
public:
    WordleChecker() : m_numGuesses(0) { loadDictionary(); }
    bool check(WordleGuess& wg, size_t& outNumGuesses);
    /*
     * true:  operation was success, answer is set
     * false: input isn't valid
     */
    bool setAnswer(string answer);
    void setRandomAnswer();
private:
    void resetFrequencyMap();
    void loadDictionary(string filename="answers.txt");
    unordered_set<string> m_dict;
    string m_answer;
    size_t m_numGuesses;
    unordered_map<char, size_t> m_frequencyMap;
};
