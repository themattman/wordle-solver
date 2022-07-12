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
using buf_ptr = shared_ptr<WordleBuffer>;

class WordlistWordleSolver : public WordleSolverImpl {
public:
    using WordleSolverImpl::WordleSolverImpl;
    WordlistWordleSolver(size_t id = 0);
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
    PassthroughWordleSolver(size_t id = 0) : WordlistWordleSolver(id) {}
    string makeInitialGuess(buf_ptr wb, size_t idx) override {
        return m_selector->select(m_wordSet.begin(), m_wordSet.end(), m_wordSet.size(), m_knownCorrects, 0);
    }
    string makeSubsequentGuess(size_t guessNum, buf_ptr wb, size_t idx) override { return m_selector->select(m_wordSet.begin(), m_wordSet.end(), m_wordSet.size(), m_knownCorrects, guessNum); }
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
    TrieBasedWordleSolver(size_t id = 0);
    TrieBasedWordleSolver(const TrieBasedWordleSolver& t) {
        cout << "TBWS copy ctor" << endl;
    }
    TrieBasedWordleSolver(TrieBasedWordleSolver&& t) {
        cout << "TBWS move ctor" << endl;
    }
    TrieBasedWordleSolver& operator=(const TrieBasedWordleSolver& t) {
        cout << "TBWS copy ass" << endl;
        return *this;
    }
    TrieBasedWordleSolver& operator=(TrieBasedWordleSolver&& t) {
        cout << "TBWS move ass" << endl;
        return *this;
    }
    string makeInitialGuess(buf_ptr wb, size_t idx) override;
    string makeSubsequentGuess(size_t guessNum, buf_ptr wb, size_t idx) override;
    void processResult(const WordleGuess& guess) override;
    size_t getNumCandidates() const override {
        return m_trie->getNumCandidates();
    }
protected:
    vector<size_t> createPositionVector(const WordleGuess& allPositions, WordleResult wr);
    bool isAnotherOccurrenceNotBlack(size_t position, const WordleGuess& g) const;
    void printNumCands(const string& color) const;
    void trimGreens(const WordleGuess& g, const vector<size_t>& positions);
    void trimYellows(const WordleGuess& g, const vector<size_t>& positions);
    void trimBlacks(const WordleGuess& g, const vector<size_t>& positions);

    unique_ptr<WordleTrie> m_trie;
};
