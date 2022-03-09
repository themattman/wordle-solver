#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Selector {
public:
    Selector() = default;
    virtual string select(iterator begin, iterator end) = 0;
};

class RandomSelector : Selector {
public:
    string select(iterator begin, iterator end) override;
private:
    size_t getRandom(iterator begin, iterator end) const;
};

class EnhancedRandomSelector : RandomSelector {
public:
    string select(iterator begin, iterator end) override;
private:
    bool containsDoubleLetter(const string& word) const;
    bool isVowel(char letter) const;
    bool containsOneVowel(const string& word) const;
};

///////////////

struct WordScore {
    WordScore() : word(), score(0) {}
    string word;
    size_t score;
};

bool compareWordScores(const WordScore& w1, const WordScore& w2) {
    return w1.score < w2.score;
}

class MostCommonLetterSelector : Selector {
public:
    string select(iterator begin, iterator end) override;
private:
    string getBestCandidate() const;
    string getWordWithMostCommonLetter(char letter) const;
    char getMostCommonLetter() const;
    void computeFrequencyMap();
    char count(char letter, const string& word) const;

    iterator m_iterBegin;
    iterator m_iterEnd;
    unordered_map<char, size_t> m_frequencyMapLetter;
    unordered_map<char, size_t> m_frequencyMapWord;
    vector<WordScore> m_sortedWords; //use customSorter approach? // for rating
};
