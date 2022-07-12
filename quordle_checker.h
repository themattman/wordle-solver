#pragma once

#include "quordle_rules.h"
#include "wordle_checker.h"
#include "wordle_rules.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


class QuordleChecker {
public:
    QuordleChecker();
    /**
      * true:  operation was success, answer is set
      * false: input word not in dictionary
      * throws if dictionary empty or (guess or answer) isn't correct length
      */
    bool check(QuordleGuess& wg, size_t& outNumGuesses);

    /**
      * true:  operation was success, answer is set
      * false: input isn't valid
      */
    bool setAnswers(std::vector<std::string> answers);
    void setRandomAnswers();
private:
    // std::unordered_set<std::string> m_dict;
    // std::vector<std::string> m_answers;
    size_t m_numGuesses;
    // std::unordered_map<char, size_t> m_frequencyMap;
    std::vector<WordleChecker> m_checkers;
};
