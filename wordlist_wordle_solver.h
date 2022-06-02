#pragma once

#include "wordle_rules.h"
#include "wordle_selector.h"
#include "wordle_solver.h"
#include "wordle_trie.h"

#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


class WordlistWordleSolver : public WordleSolverImpl {
public:
    using WordleSolverImpl::WordleSolverImpl;
    WordlistWordleSolver();
    size_t getNumCandidates() const override { return m_wordlist.size(); }
protected:
    void loadWordList(function<void(string)> eachLineCallbackActor);

    WordleSelector<SetIterator>* m_selector;
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
        return m_selector->select(m_wordSet.begin(), m_wordSet.end(), m_wordSet.size(), m_knownCorrects, 0);
    }
    string makeSubsequentGuess(size_t guessNum) override { return m_selector->select(m_wordSet.begin(), m_wordSet.end(), m_wordSet.size(), m_knownCorrects, guessNum); }
    void processResult(const WordleGuess& guess) override {}
};

//////////////////

/**
Word Trie
                             (root)
                            /  | ..  \
                         /     |   ..  \
                      /        |    ..   \
1st letter:          a         b     ..    z
                   / | \     / | \       / | \
2nd letter:      b   c  z   a  e  y     e  i  o

...

5th letter:      ...

 */

class TrieBasedWordleSolver : public PassthroughWordleSolver {
public:
    using PassthroughWordleSolver::PassthroughWordleSolver;
    TrieBasedWordleSolver();
    string makeInitialGuess() override;
    string makeSubsequentGuess(size_t guessNum) override;
    void processResult(const WordleGuess& guess) override;
    size_t getNumCandidates() const override { return m_trie->getNumCandidates(); }
protected:
    vector<size_t> createPositionVector(const WordleGuess& allPositions, WordleResult wr);
    bool isAnotherOccurrenceNotBlack(size_t position, const WordleGuess& g) const;
    void printNumCands(const string& color) const;
    void trimGreens(const WordleGuess& g, const vector<size_t>& positions);
    void trimYellows(const WordleGuess& g, const vector<size_t>& positions);
    void trimBlacks(const WordleGuess& g, const vector<size_t>& positions);

    WordleTrie* m_trie;
};
