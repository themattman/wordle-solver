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
    WordlistWordleSolver() {
        m_trie = new WordleTrie();
        loadWordList();
    }
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

class RandomWordleSolver : public WordlistWordleSolver {
public:
    RandomWordleSolver() : WordlistWordleSolver() {}
    string makeInitialGuess();
    string makeSubsequentGuess() { return makeInitialGuess(); }
protected:
    size_t getRandomNumber() { return rand() % m_wordlist.size(); }
    bool containsDoubleLetter(string word);
    bool isVowel(char letter);
    bool containsOneVowel(string word);
};

///////////////////

class NaiveRandomWordleSolver : public RandomWordleSolver {
public:
    NaiveRandomWordleSolver() : RandomWordleSolver() {}
    void processResult(const WordleGuess& guess);
};

///////////////////

class RandomPlusWordleSolver : public RandomWordleSolver {
public:
    RandomPlusWordleSolver();
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

class TrieBasedWordleSolver : public RandomWordleSolver {
public:
    TrieBasedWordleSolver() = default;
    string makeInitialGuess() override;
    string makeSubsequentGuess() override;
    void processResult(const WordleGuess& guess) override;
protected:
    using WordlistWordleSolver::getRandomNumber;
    vector<size_t> createPositionVector(const vector<WordleResult>& allPositions, WordleResult wr) const;
    void trimGreens(WordleGuess g, const vector<size_t>& positions);
    void trimYellows(WordleGuess g, const vector<size_t>& positions);
    void trimBlacks(WordleGuess g, const vector<size_t>& positions);
};
