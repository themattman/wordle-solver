#include "wordle_selectors.h"

#include <iostream>
#include <iterator>
#include <string>

using namespace std;

// TODO: begin & end
string RandomSelector::select(ForwardIterator begin, ForwardIterator end) {
    cout << "select0(" << *begin << endl;
    advance(begin, getRandom(begin, end));
    cout << "select1(" << *begin << endl;
    return *begin;
    // return *(begin + getRandom(begin, end));
}

// string RandomSelector::select(SetIterator begin, SetIterator end, size_t rangeSize) {
//     advance(begin, getRandom(begin, end, rangeSize));
//     return *begin;
//     // return *(begin + getRandom(begin, end, rangeSize));
// }

size_t RandomSelector::getRandom(ForwardIterator begin, ForwardIterator end) const {
    size_t numElements = end-begin;
    cout << "numElems:" << numElements << "|" << rand() << endl;
    return rand() % numElements;
}

// size_t RandomSelector::getRandom(SetIterator begin, SetIterator end, size_t rangeSize) const {
//     srand(time(NULL));
//     return rand() % rangeSize;
// }

string EnhancedRandomSelector::select(ForwardIterator begin, ForwardIterator end) {
    string selection;
    do {
        selection = RandomSelector::select(begin, end);
    } while (containsDoubleLetter(selection) && containsOneVowel(selection));
    return selection;
}

// string EnhancedRandomSelector::select(SetIterator begin, SetIterator end, size_t rangeSize) {
//     string selection;
//     do {
//         selection = RandomSelector::select(begin, end, rangeSize);
//     } while (containsDoubleLetter(selection) && containsOneVowel(selection));
//     return selection;
// }

bool EnhancedRandomSelector::containsDoubleLetter(const string& word) const {
    for (size_t i = 0; i < word.size(); i++) {
        for (size_t j = i+1; j < word.size(); j++) {
            if (word[i] == word[j]) {
                return true;
            }
        }
    }

    return false;
}

bool EnhancedRandomSelector::isVowel(char letter) const {
    return (letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u' || letter == 'y');
}

bool EnhancedRandomSelector::containsOneVowel(const string& word) const {
    size_t numVowels = 0;

    for (auto& letter : word) {
        numVowels = isVowel(letter) ? numVowels+1 : numVowels;
    }

    return numVowels <= 1;
}

////////////////

string MostCommonLetterSelector::select(ForwardIterator begin, ForwardIterator end) {
    m_iterBegin = begin;
    m_iterEnd = end;
    computeFrequencyMap();
    rateCandidates();
    return getBestCandidate();
}

string MostCommonLetterSelector::getBestCandidate() const {
    return getWordWithMostCommonLetter(getMostCommonLetter());
}

string MostCommonLetterSelector::getWordWithMostCommonLetter(char letter) const {
    size_t mostCommonLetterCount = 0;
    string wordWithMostCommonLetter;
    for (auto it = m_iterBegin; it != m_iterEnd; it++) {
        size_t letterCount = count(letter, *it);
        if (letterCount > mostCommonLetterCount) {
            mostCommonLetterCount = letterCount;
            wordWithMostCommonLetter = *it;
        }
    }
    return wordWithMostCommonLetter;
}

size_t MostCommonLetterSelector::count(char letter, const string& word) const {
    size_t count = 0;
    for (auto& c : word) {
        if (c == letter) {
            count++;
        }
    }
    return count;
}

char MostCommonLetterSelector::getMostCommonLetter() const {
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

void MostCommonLetterSelector::rateCandidates() {
    for (auto it = m_iterBegin; it != m_iterEnd; it++) {
        WordScore ws;
        ws.word = *it;
        cout << "it:" << *it << endl;
        for (auto& c : ws.word) {
            auto letterScoreIt = m_frequencyMapWord.find(c);
            if (letterScoreIt != m_frequencyMapWord.end()) {
                ws.score += letterScoreIt->second;
            }
        }
    }

    sort(m_sortedWords.begin(), m_sortedWords.end(), compareWordScores);
    auto it = m_sortedWords.begin();
    cout << "#1:" << it->word << endl;
    advance(it, 1);
    cout << "#2:" << it->word << endl;
}

void MostCommonLetterSelector::computeFrequencyMap() {
    for (auto it = m_iterBegin; it != m_iterEnd; it++) {
        unordered_set<char> lettersInWord;
        for (auto& c : *it) {
            if (m_frequencyMapLetter.find(c) == m_frequencyMapLetter.end()) {
                if (lettersInWord.find(c) == lettersInWord.end()) {
                    if (m_frequencyMapWord.find(c) == m_frequencyMapWord.end()) {
                        m_frequencyMapWord[c] = 0;
                    }
                    m_frequencyMapWord[c]++;
                }
                m_frequencyMapLetter[c] = 0;
            }
            m_frequencyMapLetter[c]++;
            lettersInWord.insert(c);
        }
    }
}
