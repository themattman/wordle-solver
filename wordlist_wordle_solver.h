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
    WordlistWordleSolver(std::unique_ptr<WordleSelector<SetIterator>>& selector);
    size_t getNumCandidates() const override { return m_wordlist.size(); }
protected:
    void loadWordList(function<void(string)> eachLineCallbackActor);

    vector<string> m_wordlist;
    set<string> m_wordSet;
    vector<
        unordered_map<char, vector<string>>
        > m_letterMaps;
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

class TrieBasedWordleSolver : public WordlistWordleSolver {
public:
    using WordlistWordleSolver::WordlistWordleSolver;
    TrieBasedWordleSolver(std::unique_ptr<WordleSelector<SetIterator>>& selector);
    string makeInitialGuess(buf_ptr wb, size_t idx) override;
    string makeSubsequentGuess(size_t guessNum, buf_ptr wb, size_t idx) override;
    void processResult(const WordleGuess& guess) override;
    size_t getNumCandidates() const override { return m_trie->getNumCandidates(); }
protected:
    vector<size_t> createPositionVector(const WordleGuess& allPositions, WordleResult wr);
    bool isAnotherOccurrenceNotBlack(size_t position, const WordleGuess& g) const;
    void printNumCandidates(const string& color) const;
    void trimGreenCandidates(const WordleGuess& g, const vector<size_t>& positions);
    void trimYellowCandidates(const WordleGuess& g, const vector<size_t>& positions);
    void trimBlackCandidates(const WordleGuess& g, const vector<size_t>& positions);

    WordleTrie* m_trie;
};
