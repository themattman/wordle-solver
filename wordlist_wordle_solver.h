#pragma once

#include "wordle_solver.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


class WordlistWordleSolver : public WordleSolver {
public:
    WordlistWordleSolver() { loadWordList(); }
protected:
    void loadWordList();

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
    vector<size_t> createPositionVector(const vector<WordleResult>& allPositions, WordleResult wr);
    void trimGreens(WordleGuess g, const vector<size_t>& positions);
    void trimYellows(WordleGuess g, const vector<size_t>& positions);
    void trimBlacks(WordleGuess g, const vector<size_t>& positions);
    void excludeFromSet(char excludeChar, size_t letterPosition);
    void includeInSet(char includeChar, size_t letterPosition);

    vector<set<string>> m_workingSets;
};
