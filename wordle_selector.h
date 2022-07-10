#pragma once

#include "wordle_rules.h"

#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <unordered_map>

using namespace std;


using ForwardIterator = vector<string>::iterator;
using SetIterator = set<string>::iterator;

template <typename IterType>
class WordleSelector {
public:
    WordleSelector(size_t id)
        : m_id(id) { srand(time(nullptr)); }
    virtual string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns, size_t guessNum) = 0;
protected:
    size_t m_id;
    size_t m_guessNum{0};
};

template <typename IterType>
class RandomWordleSelector : public WordleSelector<IterType> {
public:
    using WordleSelector<IterType>::WordleSelector;
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns, size_t guessNum) override;
private:
    size_t getRandom(IterType begin, IterType end, size_t rangeSize) const;
};

template <typename IterType>
class EnhancedRandomWordleSelector : public RandomWordleSelector<IterType> {
public:
    using RandomWordleSelector<IterType>::RandomWordleSelector;
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns, size_t guessNum) override;
private:
    bool containsDoubleLetter(const string& word) const;
    bool isVowel(char letter) const;
    bool containsOneVowel(const string& word) const;
};

///////////////

struct WordScore {
    WordScore() : word(), lowestLetterScore(0), score(0) {}
    string word;
    size_t lowestLetterScore;
    size_t score;
};

struct WordScoreComp {
    bool operator() (const WordScore& w1, const WordScore& w2) const {
        return w1.score > w2.score;
    }
};

/////////////////

template <typename IterType>
class MostCommonLetterWordleSelector : public WordleSelector<IterType> {
public:
    using WordleSelector<IterType>::WordleSelector;
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns, size_t guessNum) override;
protected:
    bool containsAllHints(const string& word) const;
    string getBestCandidate() const;
    void clearOldState();
    char getMostCommonLetter() const;
    void computeFrequencyMap();
    void sortWordsByFrequency();
    void printCandidates() const;
    void printLogPrefix() const;
    virtual void computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                             unordered_map<string, size_t>& wordScore) = 0;

    IterType m_iterBegin;
    IterType m_iterEnd;
    unordered_map<char, size_t> m_frequencyMapLetter;
    unordered_map<string, size_t> m_wordScore;
    vector<WordleKnown> m_knowns;
    unordered_map<char, size_t> m_alphabetFrequencyMapLetter;
    unordered_map<string, size_t> m_alphabetWordScore;
    set<WordScore, WordScoreComp> m_sortedWords{};
};

template <typename IterType>
class NaiveMostCommonLetterWordleSelector : public MostCommonLetterWordleSelector<IterType> {
public:
    using MostCommonLetterWordleSelector<IterType>::MostCommonLetterWordleSelector;
protected:
    void computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                     unordered_map<string, size_t>& wordScore) override;
};

template <typename IterType>
class ImprovedMostCommonLetterWordleSelector : public MostCommonLetterWordleSelector<IterType> {
public:
    using MostCommonLetterWordleSelector<IterType>::MostCommonLetterWordleSelector;
protected:
    void computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                     unordered_map<string, size_t>& wordScore) override;
};

template <typename IterType>
class PositionalLetterWordleSelector : public MostCommonLetterWordleSelector<IterType> {
public:
    using MostCommonLetterWordleSelector<IterType>::MostCommonLetterWordleSelector;
protected:
    void computeFrequencyMapInternal(unordered_map<char, size_t>& unused_letterMap,
                                     unordered_map<string, size_t>& wordScore) override;
    void clearOldState();

    bool m_initialGuess{true};
    vector<unordered_map<char, size_t>> m_positionLetterScores;
};

template <typename IterType>
class FrequencyAndPositionalLetterWordleSelector : public PositionalLetterWordleSelector<IterType> {
public:
    using PositionalLetterWordleSelector<IterType>::PositionalLetterWordleSelector;
protected:
    void computeFrequencyMapInternal(unordered_map<char, size_t>& unused_letterMap,
                                     unordered_map<string, size_t>& wordScore) override;
    bool m_initialGuess{true};
    vector<unordered_map<char, size_t>> m_positionLetterScores;
    unordered_map<char, size_t> m_fullLetterMap;
};

//////////

enum class WordleSelectorType {
    Random,
    EnhancedRandom,
    NaiveMostCommonLetter,
    ImprovedMostCommonLetter,
    PositionalLetter,
    FrequencyAndPositionalLetter,
};

template <typename IterType>
struct WordleSelectorFactory {
    static WordleSelector<IterType>* makeWordleSelector(const WordleSelectorType& selectorType, size_t id = 0) {
        switch (selectorType) {
        case WordleSelectorType::Random:
            return new RandomWordleSelector<IterType>(id);
        case WordleSelectorType::EnhancedRandom:
            return new EnhancedRandomWordleSelector<IterType>(id);
        case WordleSelectorType::NaiveMostCommonLetter:
            return new NaiveMostCommonLetterWordleSelector<IterType>(id);
        case WordleSelectorType::ImprovedMostCommonLetter:
            return new ImprovedMostCommonLetterWordleSelector<IterType>(id);
        case WordleSelectorType::PositionalLetter:
            return new PositionalLetterWordleSelector<IterType>(id);
        case WordleSelectorType::FrequencyAndPositionalLetter:
            return new FrequencyAndPositionalLetterWordleSelector<IterType>(id);
        }

        if (DEBUG) cerr << "Error: [selector] invalid selectorType" << endl;

        throw;
    }
};

// Explicity Instantiation to keep linker happy, TODO: why?
template class RandomWordleSelector<SetIterator>;
template class EnhancedRandomWordleSelector<SetIterator>;
template class NaiveMostCommonLetterWordleSelector<SetIterator>;
template class ImprovedMostCommonLetterWordleSelector<SetIterator>;
template class PositionalLetterWordleSelector<SetIterator>;
template class FrequencyAndPositionalLetterWordleSelector<SetIterator>;

template class RandomWordleSelector<ForwardIterator>;
template class EnhancedRandomWordleSelector<ForwardIterator>;
template class MostCommonLetterWordleSelector<ForwardIterator>;
template class NaiveMostCommonLetterWordleSelector<ForwardIterator>;
template class ImprovedMostCommonLetterWordleSelector<ForwardIterator>;
template class PositionalLetterWordleSelector<ForwardIterator>;
template class FrequencyAndPositionalLetterWordleSelector<ForwardIterator>;
