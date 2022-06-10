#pragma once

#include "wordle_rules.h"

#include <string>
#include <unordered_map>
#include <unordered_set>


class WordleChecker {
public:
    WordleChecker(bool isHardMode=false)
        : m_numGuessesUnused(0),
          m_isHardMode(isHardMode) {
        loadDictionary(DICTIONARY_GUESSES_FILENAME);
    }

    /**
      * true:  operation was success, answer is set
      * false: input word not in dictionary
      * throws if dictionary empty or (guess or answer) isn't correct length
      */
    bool check(WordleGuess& wg);
    bool checkEasyMode(WordleGuess& wg);
    bool checkHardMode(std::vector<WordleGuess>& wg);

    /**
      * true:  operation was success, answer is set
      * false: input isn't valid
      */
    bool setAnswer(std::string answer);
    void setRandomAnswer();
private:
    void resetFrequencyMap();
    void loadDictionary(std::string filename=DICTIONARY_FILENAME);
    bool checkHardModeInternalOneRound(const std::string& curWord, const std::string& priorWord,
                                       const std::vector<WordleResult>& priorResult);
    bool checkHardModeInternal(const std::vector<WordleGuess>& wgList);

    std::unordered_set<std::string> m_dict;
    std::string m_answer;
    size_t m_numGuessesUnused;
    bool m_isHardMode;
    std::unordered_map<char, size_t> m_frequencyMap;
    std::vector<WordleGuess> m_wgList;
};
