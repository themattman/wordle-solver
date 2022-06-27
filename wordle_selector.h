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
    WordleSelector() { srand(time(nullptr)); }
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns, size_t guessNum) { return "empty"; }
protected:
    size_t m_guessNum{0};
};

template <typename IterType>
class RandomWordleSelector : public WordleSelector<IterType> {
public:
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns, size_t guessNum);
private:
    size_t getRandom(IterType begin, IterType end, size_t rangeSize) const;
};

template <typename IterType>
class EnhancedRandomWordleSelector : public RandomWordleSelector<IterType> {
public:
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns, size_t guessNum);
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
    string select(IterType begin, IterType end, size_t rangeSize, const vector<WordleKnown>& knowns, size_t guessNum);
protected:
    bool isInRange(const string& word) const;
    string getBestCandidate(bool isInOriginalDictionary) const;
    void clearOldState();
    char getMostCommonLetter() const;
    void computeFrequencyMap();
    void sortWordsByFrequency();
    void printCandidates() const;
    void computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                     unordered_map<string, size_t>& wordScore);

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
protected:
    void computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                     unordered_map<string, size_t>& wordScore);
};

template <typename IterType>
class ImprovedMostCommonLetterWordleSelector : public MostCommonLetterWordleSelector<IterType> {
protected:
    void computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                     unordered_map<string, size_t>& wordScore);
};

template <typename IterType>
class PositionalLetterWordleSelector : public MostCommonLetterWordleSelector<IterType> {
protected:
    void computeFrequencyMapInternal(unordered_map<char, size_t>& unused_letterMap,
                                     unordered_map<string, size_t>& wordScore);
    void clearOldState();

    bool m_initialGuess{true};
    vector<unordered_map<char, size_t>> m_positionLetterScores;
};

template <typename IterType>
class FrequencyAndPositionalLetterWordleSelector : public PositionalLetterWordleSelector<IterType> {
protected:
    void computeFrequencyMapInternal(unordered_map<char, size_t>& unused_letterMap,
                                     unordered_map<string, size_t>& wordScore);
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
    static unique_ptr<WordleSelector<IterType>> makeWordleSelector(const WordleSelectorType& selectorType) {
        switch (selectorType) {
        case WordleSelectorType::Random:
            return make_unique<RandomWordleSelector<IterType>>();
        case WordleSelectorType::EnhancedRandom:
            return make_unique<EnhancedRandomWordleSelector<IterType>>();
        case WordleSelectorType::NaiveMostCommonLetter:
            return make_unique<NaiveMostCommonLetterWordleSelector<IterType>>();
        case WordleSelectorType::ImprovedMostCommonLetter:
            return make_unique<ImprovedMostCommonLetterWordleSelector<IterType>>();
        case WordleSelectorType::PositionalLetter:
            return make_unique<PositionalLetterWordleSelector<IterType>>();
        case WordleSelectorType::FrequencyAndPositionalLetter:
            return make_unique<FrequencyAndPositionalLetterWordleSelector<IterType>>();
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

// unique_ptr<WordleSelectorBase> createWordleSelector(const string& selectorType) {
//     std::unique_ptr<WordleSelectorImpl> selectorPtr;
//     if (selectorType == "") {
//         selectorPtr = WordleSelectorFactor<SetIterator>::makeWordleSelector(Wordle);
//     } else if (selectorType == "") {
//         selectorPtr = WordleSelectorFactor<SetIterator>::makeWordleSelector();
//     } else {
//         printUsage();
//     }
//     return selectorPtr;
// }
