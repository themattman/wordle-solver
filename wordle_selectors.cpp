#include "wordle_selectors.h"

#include <string>

using namespace std;

// TODO: begin & end
string RandomSelector::select(iterator begin, iterator end) {
    return *(begin + getRandom(begin, end));
}

size_t RandomSelector::getRandom(iterator begin, iterator end) {
    srand(time(NULL));
    size_t numElements = end-begin;
    return rand() % numElements;
}

string EnhancedRandomSelector::select(iterator begin, iterator end) {
    string selection;
    do {
        selection = RandomSelector::select(begin, end);
    } while (containsDoubleLetter(selection) && containsOneVowel(selection));
    return selection;
}

bool EnhancedRandomSelector::containsDoubleLetter(const string& word) {
    for (size_t i = 0; i < word.size(); i++) {
        for (size_t j = i+1; j < word.size(); j++) {
            if (word[i] == word[j]) {
                return true;
            }
        }
    }

    return false;
}

bool EnhancedRandomSelector::isVowel(char letter) {
    return (letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u' || letter == 'y');
}

bool EnhancedRandomSelector::containsOneVowel(const string& word) {
    size_t numVowels = 0;

    for (auto& letter : word) {
        numVowels = isVowel(letter) ? numVowels+1 : numVowels;
    }

    return numVowels <= 1;
}

////////////////

string MostCommonLetterSelector::select(iterator begin, iterator end) {
    m_iterBegin = begin;
    m_iterEnd = end;
    computeFrequencyMap();
    rateCandidates();
    return getBestCandidate();
}

string MostCommonLetterSelector::getBestCandidate() {
    return getWordWithMostCommonLetter(getMostCommonLetter());
}

string MostCommonLetterSelector::getWordWithMostCommonLetter(char letter) {
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

size_t MostCommonLetterSelector::count(char letter, const string& word) {
    size_t count = 0;
    for (auto& c : word) {
        if (c == letter) {
            count++;
        }
    }
    return count;
}

char MostCommonLetterSelector::getMostCommonLetter() {
    size_t highest = 0;
    char mostCommon;
    for (auto it = m_frequencyMapLetter.begin(); it != m_frequencyMapLetter.end(); it++) {
        if (it->second > highest) {
            highest = it->second;
            mostCommon = it->first
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
        ws.word = it->first;
        for (auto& c : ws.word) {
            auto letterScoreIt = m_frequencyMapWord.find(c);
            if (letterScoreIt != m_frequencyMapWord.end()) {
                ws.score += *letterScoreIt;
            }
        }
    }
    sort(m_sortedWords.begin(), m_sortedWords.end(), compareWordScores());
    auto it = m_sortedWords.begin();
    cout << "#1:" << *it << endl;
    advance(it, 1);
    cout << "#2:" << *it << endl;
}

void MostCommonLetterSelector::computeFrequencyMap() {
    for (auto it = m_iterBegin; it != m_iterEnd(); it++) {
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
