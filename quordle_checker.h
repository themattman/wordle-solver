#pragma once

#include "quordle_rules.h"
#include "wordle_rules.h"

#include <string>
#include <unordered_map>
#include <unordered_set>


class QuordleChecker {
public:
    QuordleChecker() : m_numGuesses(0) { loadDictionary(); }
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
    bool setAnswer(std::string answer);
    void setRandomAnswer();
private:
    void resetFrequencyMap();
    void loadDictionary(std::string filename=DICTIONARY_FILENAME);

    std::unordered_set<std::string> m_dict;
    std::string m_answer;
    size_t m_numGuesses;
    std::unordered_map<char, size_t> m_frequencyMap;
};
