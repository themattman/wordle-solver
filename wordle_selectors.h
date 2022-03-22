#pragma once

#include "wordle_rules.h"

#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


using ForwardIterator = vector<string>::iterator;
using SetIterator = set<string>::iterator;

template <typename IterType>
class Selector {
public:
    Selector() { srand(time(nullptr)); }
    virtual string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns) = 0;
};

template <typename IterType>
class RandomSelector : public Selector<IterType> {
public:
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns) override;
private:
    size_t getRandom(IterType begin, IterType end, size_t rangeSize) const;
};

template <typename IterType>
class EnhancedRandomSelector : public RandomSelector<IterType> {
public:
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns) override;
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

struct WordScoreComp {
    bool operator() (const WordScore& w1, const WordScore& w2) const {
        return w1.score > w2.score;
    }
};

template <typename IterType>
class MostCommonLetterSelector : public Selector<IterType> {
public:
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns) override;
private:
    string getBestCandidate() const;
    void clearOldState();
    char getMostCommonLetter() const;
    void computeFrequencyMap();
    void computeFrequencyMapInternalIsolatedLetter(unordered_map<char, size_t>& letterMap,
                                                   unordered_map<string, size_t>& wordScore);
    void computeFrequencyMapInternalBetter(unordered_map<char, size_t>& letterMap,
                                           unordered_map<string, size_t>& wordScore);
    void computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                      unordered_map<string, size_t>& wordScore);
    void sortWordsByFrequency();
    size_t count(char letter, const string& word) const;

    bool m_uninitialized{true};
    IterType m_iterBegin;
    IterType m_iterEnd;
    unordered_map<char, size_t> m_frequencyMapLetter;
    unordered_map<string, size_t> m_wordScore;
    vector<WordleKnown> m_knowns;

    vector<unordered_map<char, size_t>> m_positionLetterScores;

    unordered_map<char, size_t> m_alphabetFrequencyMapLetter;
    unordered_map<string, size_t> m_alphabetWordScore;
    set<WordScore, WordScoreComp> m_sortedWords{};
};


//////////

enum class SelectorType {
    Random,
    EnhancedRandom,
    MostCommonLetter
};

template <typename IterType>
struct SelectorFactory {
    static Selector<IterType>* makeSelector(const SelectorType& selectorType) {
        switch (selectorType) {
        case SelectorType::Random:
            return new RandomSelector<IterType>();
        case SelectorType::EnhancedRandom:
            return new EnhancedRandomSelector<IterType>();
        case SelectorType::MostCommonLetter:
            return new MostCommonLetterSelector<IterType>();
        }
        if (DEBUG) {
            cerr << "Error: [selector] invalid selectorName" << endl;
        }
        throw;
    }
};

// Explicity Instantiation to keep linker happy, TODO: why?
template class RandomSelector<SetIterator>;
template class EnhancedRandomSelector<SetIterator>;
template class MostCommonLetterSelector<SetIterator>;

template class RandomSelector<ForwardIterator>;
template class EnhancedRandomSelector<ForwardIterator>;
template class MostCommonLetterSelector<ForwardIterator>;
