#pragma once

#include "wordle_solver.h"
#include "wordle_trie.h"

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
    void loadWordList();

    vector<string> m_wordlist;
    set<string> m_wordSet;
    vector<
        unordered_map<char, vector<string>>
        > m_letterMaps;
};

/////////////////////

class PassthroughWordleSolver : public WordlistWordleSolver {
public:
    //using WordlistWordleSolver::WordlistWordleSolver;
    PassthroughWordleSolver(Selector* s) : WordlistWordleSolver(s) {
        cout << "PTWS" << endl;
        cout << "PTWS:" << s << "|" << m_selector << endl;
    }
    string makeInitialGuess() override { cout << "initial sz:" << m_wordlist.size() << " - " << m_selector << endl;
        auto r = m_selector->select(m_wordlist.begin(), m_wordlist.end());
        cout << "done" << endl;
        return r;
    }
    string makeSubsequentGuess() override { cout << "subseq" << endl; return makeInitialGuess(); }
    void processResult(const WordleGuess& guess) override {}
};

//////////////////

class TrieBasedWordleSolver : public PassthroughWordleSolver {
public:
    TrieBasedWordleSolver(Selector* s) : PassthroughWordleSolver(s) {
        m_trie = new WordleTrie();
    };
    using PassthroughWordleSolver::PassthroughWordleSolver;
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
