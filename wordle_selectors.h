#pragma once

#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

template <typename ForwardIterator>
class Selector {
public:
    Selector() = default;
    virtual string select(ForwardIterator begin, ForwardIterator end) = 0;
};

template <typename ForwardIterator>
class RandomSelector : Selector<ForwardIterator> {
public:
    string select(ForwardIterator begin, ForwardIterator end) override;
private:
    size_t getRandom(ForwardIterator begin, ForwardIterator end) const;
};

template <typename ForwardIterator>
class EnhancedRandomSelector : RandomSelector<ForwardIterator> {
public:
    string select(ForwardIterator begin, ForwardIterator end) override;
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

template <typename ForwardIterator>
class MostCommonLetterSelector : Selector<ForwardIterator> {
public:
    string select(ForwardIterator begin, ForwardIterator end) override;
private:
    string getBestCandidate() const;
    string getWordWithMostCommonLetter(char letter) const;
    char getMostCommonLetter() const;
    void computeFrequencyMap();
    char count(char letter, const string& word) const;

    ForwardIterator m_iterBegin;
    ForwardIterator m_iterEnd;
    unordered_map<char, size_t> m_frequencyMapLetter;
    unordered_map<char, size_t> m_frequencyMapWord;
    vector<WordScore> m_sortedWords; //use customSorter approach? // for rating
};
