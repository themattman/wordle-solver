#pragma once

#include "wordle_solver.h"
#include "wordle_trie.h"

#include <functional>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


class WordlistWordleSolver : public WordleSolver {
public:
    using WordleSolver::WordleSolver;
    WordlistWordleSolver();
protected:
    void loadWordList(function<void(string)> eachLineCallbackActor);

    Selector<SetIterator>* m_selector;
    vector<string> m_wordlist;
    set<string> m_wordSet;
    vector<
        unordered_map<char, vector<string>>
        > m_letterMaps;
};

/////////////////////

class PassthroughWordleSolver : public WordlistWordleSolver {
public:
    using WordlistWordleSolver::WordlistWordleSolver;
    string makeInitialGuess() override {
        return m_selector->select(m_wordSet.begin(), m_wordSet.end(), m_wordSet.size(), m_knownCorrects);
    }
    string makeSubsequentGuess() override { return makeInitialGuess(); }
    void processResult(const WordleGuess& guess) override {}
};

//////////////////

class TrieBasedWordleSolver : public PassthroughWordleSolver {
public:
    using PassthroughWordleSolver::PassthroughWordleSolver;
    TrieBasedWordleSolver();
    string makeInitialGuess() override;
    string makeSubsequentGuess() override;
    void processResult(const WordleGuess& guess) override;
    size_t getNumCandidates() const { return m_trie->getNumCandidates(); }
protected:
    vector<size_t> createPositionVector(const WordleGuess& allPositions, WordleResult wr);
    bool isAnotherOccurrenceNotBlack(size_t position, const WordleGuess& g) const;
    void printNumCands(const string& color) const;
    void trimGreens(const WordleGuess& g, const vector<size_t>& positions);
    void trimYellows(const WordleGuess& g, const vector<size_t>& positions);
    void trimBlacks(const WordleGuess& g, const vector<size_t>& positions);
    WordleTrie* m_trie;
};
