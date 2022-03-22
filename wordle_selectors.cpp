#include "wordle_selectors.h"

#include <iostream>
#include <iterator>
#include <string>

using namespace std;

template <typename IterType>
string RandomSelector<IterType>::select(IterType begin, IterType end, size_t rangeSize,
                                        const vector<WordleKnown>& knowns) {
    advance(begin, getRandom(begin, end, rangeSize));
    return *begin;
}

template <typename IterType>
size_t RandomSelector<IterType>::getRandom(IterType begin, IterType end, size_t rangeSize) const {
    srand(time(NULL));
    return rand() % rangeSize;
}

template <typename IterType>
string EnhancedRandomSelector<IterType>::select(IterType begin, IterType end, size_t rangeSize,
                                                const vector<WordleKnown>& knowns) {
    string selection;
    do {
        selection = RandomSelector<IterType>::select(begin, end, rangeSize, knowns);
    } while (containsDoubleLetter(selection) && containsOneVowel(selection));
    return selection;
}

template <typename IterType>
bool EnhancedRandomSelector<IterType>::containsDoubleLetter(const string& word) const {
    for (size_t i = 0; i < word.size(); i++) {
        for (size_t j = i+1; j < word.size(); j++) {
            if (word[i] == word[j]) {
                return true;
            }
        }
    }

    return false;
}

template <typename IterType>
bool EnhancedRandomSelector<IterType>::isVowel(char letter) const {
    return (letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u' || letter == 'y');
}

template <typename IterType>
bool EnhancedRandomSelector<IterType>::containsOneVowel(const string& word) const {
    size_t numVowels = 0;

    for (auto& letter : word) {
        numVowels = isVowel(letter) ? numVowels+1 : numVowels;
    }

    return numVowels <= 1;
}

////////////////

template <typename IterType>
string MostCommonLetterSelector<IterType>::select(IterType begin, IterType end, size_t rangeSize,
                                                  const vector<WordleKnown>& knowns) {
    clearOldState();
    m_knowns = knowns;
    m_iterBegin = begin;
    m_iterEnd = end;
    computeFrequencyMap();
    return getBestCandidate();
}

template <typename IterType>
void MostCommonLetterSelector<IterType>::clearOldState() {
    m_knowns.clear();
    m_frequencyMapLetter.clear();
    m_wordScore.clear();
    m_sortedWords.clear();
}

template <typename IterType>
string MostCommonLetterSelector<IterType>::getBestCandidate() const {
    return m_sortedWords.begin()->word;
}

template <typename IterType>
char MostCommonLetterSelector<IterType>::getMostCommonLetter() const {
    size_t highest = 0;
    char mostCommon = '0';
    for (auto it = m_frequencyMapLetter.begin(); it != m_frequencyMapLetter.end(); it++) {
        if (it->second > highest) {
            highest = it->second;
            mostCommon = it->first;
        }
    }

    if (!mostCommon) {
        if (DEBUG) {
            cerr << "Error: [selector] no most common letter" << endl;
        }
        throw;
    }

    return mostCommon;
}

template <typename IterType>
void MostCommonLetterSelector<IterType>::computeFrequencyMap() {
    if (m_alphabetWordScore.size() == 0) {
        computeFrequencyMapInternal(m_alphabetFrequencyMapLetter, m_alphabetWordScore);
        m_frequencyMapLetter = m_alphabetFrequencyMapLetter;
        m_wordScore = m_alphabetWordScore;
    } else {
        computeFrequencyMapInternal(m_frequencyMapLetter, m_wordScore);
    }
    sortWordsByFrequency();
}

template <typename IterType>
void MostCommonLetterSelector<IterType>::sortWordsByFrequency() {
    for (auto wordIt = m_wordScore.begin(); wordIt != m_wordScore.end(); wordIt++) {
        auto ws = WordScore();
        ws.word = wordIt->first;
        ws.score = wordIt->second;
        m_sortedWords.insert(ws);
    }
}

template <typename IterType>
void NaiveMostCommonLetterSelector<IterType>::computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                                                          unordered_map<string, size_t>& wordScore) {
    // Compute letter scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        for (auto& c : *wordIt) {
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
        }
    }

    // Compute word scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        for (auto& c : *wordIt) {
            if (wordLetters.find(c) == wordLetters.end()) {
                wordLetters.insert(c);
                score += letterMap[c];
            }
        }
        wordScore.insert({*wordIt, score});
    }
}

template <typename IterType>
void ImprovedMostCommonLetterSelector<IterType>::computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                                                             unordered_map<string, size_t>& wordScore) {
    // Compute letter scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        for (auto& c : *wordIt) {
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
        }
    }

    // Compute word scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        set<char> greenLetters;
        for (size_t i = 0; i < this->m_knowns.size(); i++) {
            if (this->m_knowns[i].result == WordleResult::GREEN) {
                greenLetters.insert(this->m_knowns[i].letter);
            }
        }
        size_t i = 0;
        for (auto& c : *wordIt) {
            // only give scores to each letter once
            if (wordLetters.find(c) == wordLetters.end()) {
                // only give scores to each letter that isn't already known
                if (this->m_knowns[i].result != WordleResult::GREEN) {
                    if (greenLetters.find(c) == greenLetters.end()) {
                        score += letterMap[c];
                    }
                }
            }
            wordLetters.insert(c);
            i++;
        }
        wordScore.insert({*wordIt, score});
    }
}

template <typename IterType>
void PositionalLetterSelector<IterType>::computeFrequencyMapInternal(unordered_map<char, size_t>& unused_letterMap,
                                                                     unordered_map<string, size_t>& wordScore) {
    // Compute letter scores
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        auto letterMap = unordered_map<char, size_t>();
        size_t biggestScore = 0;
        char biggestLetter = '0';
        for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
            char c = (*wordIt)[i];
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
            if (letterMap[c] > biggestScore) {
                biggestScore = letterMap[c];
                biggestLetter = c;
            }
        }
        m_positionLetterScores.push_back(letterMap);
    }

    // Compute word scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        set<char> greenLetters;
        for (size_t i = 0; i < this->m_knowns.size(); i++) {
            if (this->m_knowns[i].result == WordleResult::GREEN) {
                greenLetters.insert(this->m_knowns[i].letter);
            }
        }
        size_t i = 0;
        for (auto& c : *wordIt) {
            // only give scores to each letter once
            if (wordLetters.find(c) == wordLetters.end()) {
                // only give scores to each letter that isn't already known
                if (this->m_knowns[i].result != WordleResult::GREEN) {
                    if (greenLetters.find(c) == greenLetters.end()) {
                        score += m_positionLetterScores[i][c];
                    }
                }
            }
            wordLetters.insert(c);
            i++;
        }
        wordScore.insert({*wordIt, score});
    }
}

template <typename IterType>
void PositionalLetterSelector<IterType>::clearOldState() {
    MostCommonLetterSelector<IterType>::clearOldState();
    m_positionLetterScores.clear();
}
