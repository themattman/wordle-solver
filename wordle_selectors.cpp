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
    m_positionLetterScores.clear();
}

template <typename IterType>
string MostCommonLetterSelector<IterType>::getBestCandidate() const {
    //cout << "getBestCand(" << m_sortedWords.size() << "):" << m_sortedWords.begin()->word << ":" << m_sortedWords.begin()->score << endl;
    auto it = m_sortedWords.begin();
    //cout << "getBestCand:" << it->word << ":" << it->score << endl;
    size_t bestScore = it->score;
    advance(it, 1);
    if (it != m_sortedWords.end()) {
        //cout << "getBestCand():" << it->word << ":" << it->score << endl;
        if (bestScore == it->score) {
            cout << "Tie!" << endl;
        }
    }
    return m_sortedWords.begin()->word;
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
void MostCommonLetterSelector<IterType>::computeFrequencyMap() {
    if (m_alphabetWordScore.size() == 0) {
        computeFrequencyMapInternalIsolatedLetter(m_alphabetFrequencyMapLetter, m_alphabetWordScore);
        m_frequencyMapLetter = m_alphabetFrequencyMapLetter;
        m_wordScore = m_alphabetWordScore;
    } else {
        computeFrequencyMapInternalIsolatedLetter(m_frequencyMapLetter, m_wordScore);
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
void MostCommonLetterSelector<IterType>::computeFrequencyMapInternalIsolatedLetter(unordered_map<char, size_t>& unused_letterMap,
                                                                                   unordered_map<string, size_t>& wordScore) {
    // Compute letter scores
    //cout << "scores>:";
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        auto letterMap = unordered_map<char, size_t>();
        size_t biggestScore = 0;
        char biggestLetter = '0';
        for (auto wordIt = m_iterBegin; wordIt != m_iterEnd; wordIt++) {
            char c = (*wordIt)[i];
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
            if (letterMap[c] > biggestScore) {
                biggestScore = letterMap[c];
                biggestLetter = c;
            }
            // cout << c << "|" << *wordIt << "|" << letterMap[c] << endl;
        }
        //cout << biggestLetter;
        m_positionLetterScores.push_back(letterMap);
    }
    //cout << endl;

    // Compute word scores
    for (auto wordIt = m_iterBegin; wordIt != m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        set<char> greenLetters;
        for (size_t i = 0; i < m_knowns.size(); i++) {
            if (m_knowns[i].result == WordleResult::GREEN) {
                greenLetters.insert(m_knowns[i].letter);
            }
        }
        size_t i = 0;
        for (auto& c : *wordIt) {
            // cout << "letter:" << c << ":" << letterMap[c] << endl;

            // only give scores to each letter once
            if (wordLetters.find(c) == wordLetters.end()) {
                // only give scores to each letter that isn't already known
                if (m_knowns[i].result != WordleResult::GREEN) {
                    if (greenLetters.find(c) == greenLetters.end()) {
                        // cout << "lscore[" << c << "]=" << m_positionLetterScores[i][c] << endl;
                        score += m_positionLetterScores[i][c];
                    }
                }
            }
            wordLetters.insert(c);
            i++;
        }
        // cout << "word:" << *wordIt << ":" << score << endl;
        wordScore.insert({*wordIt, score});
    }

    // cout << "~~letterMap~~" << endl;
    // for (auto it = letterMap.begin(); it != letterMap.end(); it++) {
    //     cout << it->first << ":" << it->second << endl;
    // }
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
        set<char> greenLetters;
        for (size_t i = 0; i < m_knowns.size(); i++) {
            if (m_knowns[i].result == WordleResult::GREEN) {
                greenLetters.insert(m_knowns[i].letter);
            }
        }
        size_t i = 0;
        for (auto& c : *wordIt) {
            // cout << "letter:" << c << ":" << letterMap[c] << endl;

            // only give scores to each letter once
            if (wordLetters.find(c) == wordLetters.end()) {
                // only give scores to each letter that isn't already known
                if (m_knowns[i].result != WordleResult::GREEN) {
                    if (greenLetters.find(c) == greenLetters.end()) {
                        score += letterMap[c];
                    // } else {
                    //     //score += letterMap[c]/8;
                    //     //cout << "no score, letter already green:" << c << "-" << *wordIt << "newscore:" << letterMap[c
                        // ]/2 << endl;
                    }
                }
            // } else {
                // cout << "not incrementing on 2nd occur of letter:" << c << "-" << *wordIt << endl;
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
            if (wordLetters.find(c) == wordLetters.end()) {
                wordLetters.insert(c);
                score += letterMap[c];
            }
        }
        wordScore.insert({*wordIt, score});
    }
}
