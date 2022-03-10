#pragma once

#include "wordle_solver.h"
#include "wordle_trie.h"

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


template <typename FwdIter>
class WordlistWordleSolver : public WordleSolver<FwdIter> {
public:
    // TODO: how to inherit ctor from PV base class
    WordlistWordleSolver(Selector<FwdIter>* s);
protected:
    void loadWordList();
    size_t getRandomNumber() { return rand() % m_trie->getNumCandidates(); }

    WordleTrie* m_trie;
    vector<string> m_wordlist;
    set<string> m_wordSet;
    vector<
        unordered_map<char, vector<string>>
        > m_letterMaps;
};

/////////////////////

template <typename FwdIter>
class RandomWordleSolver : public WordlistWordleSolver<FwdIter> {
public:
    string makeInitialGuess() override;
    string makeSubsequentGuess() override { return makeInitialGuess(); }
protected:
    size_t getRandomNumber() const { return rand() % WordlistWordleSolver<FwdIter>::m_wordlist.size(); }
};

///////////////////

template <typename FwdIter>
class NaiveRandomWordleSolver : public RandomWordleSolver<FwdIter> {
public:
    void processResult(const WordleGuess& guess) override;
};

///////////////////

template <typename FwdIter>
class RandomPlusWordleSolver : public RandomWordleSolver<FwdIter> {
public:
    void processResult(const WordleGuess& guess) override;
protected:
    vector<size_t> createPositionVector(const vector<WordleResult>& allPositions, WordleResult wr) const;
    void trimGreens(WordleGuess g, const vector<size_t>& positions);
    void trimYellows(WordleGuess g, const vector<size_t>& positions);
    void trimBlacks(WordleGuess g, const vector<size_t>& positions);
    void excludeFromSet(char excludeChar, size_t letterPosition);
    void includeInSet(char includeChar, size_t letterPosition);
    vector<string> processIntersectionOfIncludes();
    void printVector(const vector<string>& v, const string& s) const;
    void propagateIncludes(const WordleGuess& wg, const vector<string>& refinedResult);

    vector<set<string>> m_workingSets;
};

//////////////////

template <typename FwdIter>
class TrieBasedWordleSolver : public RandomWordleSolver<FwdIter> {
public:
    //using WordleSolver::WordleSolver;
    // TrieBasedWordleSolver(Selector* s) {}
    string makeInitialGuess() override;
    string makeSubsequentGuess() override;
    void processResult(const WordleGuess& guess) override;
protected:
    using WordlistWordleSolver<FwdIter>::getRandomNumber;
    vector<size_t> createPositionVector(const vector<WordleResult>& allPositions, WordleResult wr) const;
    void trimGreens(WordleGuess g, const vector<size_t>& positions);
    void trimYellows(WordleGuess g, const vector<size_t>& positions);
    void trimBlacks(WordleGuess g, const vector<size_t>& positions);
};
