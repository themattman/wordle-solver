#pragma once

#include "wordle_rules.h"
#include "wordlist_wordle_solver.h"

#include <string>
#include <utility>
#include <vector>

using namespace std;

class QuordleSolver {
 public:
    QuordleSolver(const string& solverType, size_t numSolvers);
    void processResults(const vector<WordleGuess>& guesses);
    string makeGuess(size_t numGuess, buf_ptr wb);
    size_t getNumCandidates() const { return m_solvers[0]->getNumCandidates(); }
    vector<bool> m_isInPlay;
 private:
    void printRemainingCandidates() const;
    vector<unique_ptr<TrieBasedWordleSolver>> m_solvers;
    vector<size_t> m_numCandidatesList;
    pair<size_t, size_t> m_lowestCountAndIndex; // 0: size, 1: index
    size_t m_numSolvers;
    size_t m_initCandidates;
    //size_t m_numSolved{0};
};
