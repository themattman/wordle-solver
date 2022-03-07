#pragma once

#include "wordle_rules.h"

#include <string>
#include <unordered_set>

using namespace std;


class WordleChecker {
public:
    WordleChecker() = default;
    bool check(WordleGuess& wg);
    /*
     * true:  operation was success, answer is set
     * false: input isn't valid
     */
    bool setAnswer(string answer);
private:
    void loadDictionary(string filename="answers.txt");
    unordered_set<string> m_dict;
    string m_answer;
};
