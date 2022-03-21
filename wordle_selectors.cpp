#include "wordle_selectors.h"

#include <iostream>
#include <iterator>
#include <string>

using namespace std;

template <typename IterType>
string RandomSelector<IterType>::select(IterType begin, IterType end, size_t rangeSize,
                                        const vector<WordleResult>& knowns) {
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
                                                const vector<WordleResult>& knowns) {
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
                                                  const vector<WordleResult>& knowns) {
    //cout << "select(empty):" << rangeSize << endl;
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
    // cout << "getBestCand(" << m_sortedWords.size() << "):" << m_sortedWords.begin()->word << ":" << m_sortedWords.begin()->score << endl;
    auto it = m_sortedWords.begin();
    advance(it, 1);
    // cout << "getBestCand:" << it->word << ":" << it->score << endl;
    return m_sortedWords.begin()->word;
    //return getWordWithMostCommonLetter(); //getMostCommonLetter());
}

template <typename IterType>
string MostCommonLetterSelector<IterType>::getWordWithMostCommonLetter() const {
    return m_sortedWords.begin()->word;
    // size_t mostCommonLetterCount = 0;
    // string wordWithMostCommonLetter;
    // for (auto it = m_iterBegin; it != m_iterEnd; it++) {
    //     size_t letterCount = count(letter, *it);
    //     if (letterCount > mostCommonLetterCount) {
    //         mostCommonLetterCount = letterCount;
    //         wordWithMostCommonLetter = *it;
    //     }
    // }
    // return wordWithMostCommonLetter;
}

template <typename IterType>
size_t MostCommonLetterSelector<IterType>::count(char letter, const string& word) const {
    size_t count = 0;
    for (auto& c : word) {
        if (c == letter) {
            count++;
        }
    }
    return count;
}

template <typename IterType>
char MostCommonLetterSelector<IterType>::getMostCommonLetter() const {
    size_t highest = 0;
    char mostCommon;
    for (auto it = m_frequencyMapLetter.begin(); it != m_frequencyMapLetter.end(); it++) {
        if (it->second > highest) {
            highest = it->second;
            mostCommon = it->first;
        }
    }

    if (!mostCommon) {
        throw;
    }

    return mostCommon;
}

template <typename IterType>
void MostCommonLetterSelector<IterType>::rateCandidates() {
    // for (auto it = m_iterBegin; it != m_iterEnd; it++) {
    //     WordScore ws;
    //     ws.word = *it;
    //     for (auto& c : ws.word) {
    //         auto letterScoreIt = m_frequencyMapWord.find(c);
    //         if (letterScoreIt != m_frequencyMapWord.end()) {
    //             ws.score += letterScoreIt->second;
    //         }
    //     }
    // }

    // sort(m_sortedWords.begin(), m_sortedWords.end(), compareWordScores);
    // auto it = m_sortedWords.begin();
    // advance(it, 1);
}

template <typename IterType>
void MostCommonLetterSelector<IterType>::computeFrequencyMap() {
    // cout << "computeFreqMap" << endl;
    if (m_alphabetFrequencyMapLetter.size() == 0) {
        // cout << "firsttime" << endl;
        computeFrequencyMapInternalBetter(m_alphabetFrequencyMapLetter, m_alphabetWordScore);
        m_frequencyMapLetter = m_alphabetFrequencyMapLetter;
        m_wordScore = m_alphabetWordScore;
    } else {
        // cout << "not firsttime" << endl;
        computeFrequencyMapInternalBetter(m_frequencyMapLetter, m_wordScore);
    }
    sortWordsByFrequency();
}

template <typename IterType>
void MostCommonLetterSelector<IterType>::sortWordsByFrequency() {
    //cout << "sortWordsByFreq:" << m_wordScore.size() << endl;
    for (auto wordIt = m_wordScore.begin(); wordIt != m_wordScore.end(); wordIt++) {
        auto ws = WordScore();
        ws.word = wordIt->first;
        ws.score = wordIt->second;
        m_sortedWords.insert(ws);
    }
}


template <typename IterType>
void MostCommonLetterSelector<IterType>::computeFrequencyMapInternalBetter(unordered_map<char, size_t>& letterMap,
                                                                           unordered_map<string, size_t>& wordScore) {
    // Compute letter scores
    for (auto wordIt = m_iterBegin; wordIt != m_iterEnd; wordIt++) {
        for (auto& c : *wordIt) {
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
        }
    }

    // Compute word scores
    for (auto wordIt = m_iterBegin; wordIt != m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        size_t i = 0;
        for (auto& c : *wordIt) {
            // cout << "letter:" << c << ":" << letterMap[c] << endl;
            if (wordLetters.find(c) == wordLetters.end() && m_knowns[i] != WordleResult::GREEN) {
                score += letterMap[c];
            // } else {
            //     cout << "not incrementing on 2nd occur of letter:" << c << endl;
            }
            wordLetters.insert(c);
            i++;
        }
        //cout << "word:" << *wordIt << ":" << score << endl;
        wordScore.insert({*wordIt, score});
    }

    // cout << "~~letterMap~~" << endl;
    // for (auto it = letterMap.begin(); it != letterMap.end(); it++) {
    //     cout << it->first << ":" << it->second << endl;
    // }
}

template <typename IterType>
void MostCommonLetterSelector<IterType>::computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                                                     unordered_map<string, size_t>& wordScore) {
    // cout << "~~letter0Map~~" << endl;
    // for (auto it = letterMap.begin(); it != letterMap.end(); it++) {
    //     cout << it->first << ":" << it->second << endl;
    // }
    // cout << "==letter0Map==" << endl;

    // Compute letter scores
    for (auto wordIt = m_iterBegin; wordIt != m_iterEnd; wordIt++) {
        for (auto& c : *wordIt) {
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
        }
    }

    // Compute word scores
    for (auto wordIt = m_iterBegin; wordIt != m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        for (auto& c : *wordIt) {
            // cout << "letter:" << c << ":" << letterMap[c] << endl;
            if (wordLetters.find(c) == wordLetters.end()) {
                wordLetters.insert(c);
                score += letterMap[c];
            // } else {
            //     cout << "not incrementing on 2nd occur of letter:" << c << endl;
            }
        }
        // cout << "word:" << *wordIt << ":" << score << endl;
        wordScore.insert({*wordIt, score});
    }

    // cout << "~~letterMap~~" << endl;
    // for (auto it = letterMap.begin(); it != letterMap.end(); it++) {
    //     cout << it->first << ":" << it->second << endl;
    // }
}

/*
iterate over every letter of every word
bump letterMap
now have computed score for every letter


 */
