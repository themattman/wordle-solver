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
    // TODO: how to inherit ctor from PV base class
    using WordleSolver::WordleSolver;
    WordlistWordleSolver(Selector* s);
protected:
    void loadWordList(function<void(string)> eachLineCallbackActor);

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
    string makeInitialGuess() override { return m_selector->select(m_wordlist.begin(), m_wordlist.end()); }
    string makeSubsequentGuess() override { return makeInitialGuess(); }
    void processResult(const WordleGuess& guess) override {}
};

//////////////////

class TrieBasedWordleSolver : public PassthroughWordleSolver {
public:
    using PassthroughWordleSolver::PassthroughWordleSolver;
    TrieBasedWordleSolver(Selector* s);
    string makeInitialGuess() override;
    string makeSubsequentGuess() override;
    void processResult(const WordleGuess& guess) override;
protected:
    vector<size_t> createPositionVector(const vector<WordleResult>& allPositions, WordleResult wr) const;
    void trimGreens(WordleGuess g, const vector<size_t>& positions);
    void trimYellows(WordleGuess g, const vector<size_t>& positions);
    void trimBlacks(WordleGuess g, const vector<size_t>& positions);
    WordleTrie* m_trie;
};
