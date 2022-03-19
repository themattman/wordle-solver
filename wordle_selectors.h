#pragma once

#include <iterator>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

using ForwardIterator = vector<string>::iterator;
using SetIterator = unordered_set<string>::iterator;

class Selector {
public:
    Selector() = default;
    virtual string select(ForwardIterator begin, ForwardIterator end) = 0;
    virtual string select(SetIterator begin, SetIterator end, size_t rangeSize) = 0;
};

class RandomSelector : public Selector {
public:
    string select(ForwardIterator begin, ForwardIterator end) override;
    string select(SetIterator begin, SetIterator end, size_t rangeSize) override;
private:
    size_t getRandom(ForwardIterator begin, ForwardIterator end) const;
    size_t getRandom(SetIterator begin, SetIterator end, size_t rangeSize) const;
};

class EnhancedRandomSelector : public RandomSelector {
public:
    string select(ForwardIterator begin, ForwardIterator end) override;
    string select(SetIterator begin, SetIterator end, size_t rangeSize) override;
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

inline bool compareWordScores(const WordScore& w1, const WordScore& w2) {
    return w1.score < w2.score;
}

class MostCommonLetterSelector : public Selector {
public:
    string select(ForwardIterator begin, ForwardIterator end) override;
private:
    string getBestCandidate() const;
    string getWordWithMostCommonLetter(char letter) const;
    char getMostCommonLetter() const;
    void rateCandidates();
    void computeFrequencyMap();
    size_t count(char letter, const string& word) const;

    ForwardIterator m_iterBegin;
    ForwardIterator m_iterEnd;
    unordered_map<char, size_t> m_frequencyMapLetter;
    unordered_map<char, size_t> m_frequencyMapWord;
    vector<WordScore> m_sortedWords; //use customSorter approach? // for rating
};
