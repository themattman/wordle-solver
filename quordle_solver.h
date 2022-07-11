#pragma once

#include "wordle_rules.h"
#include "wordle_solver.h"

#include <string>
#include <utility>
#include <vector>

using namespace std;

class QuordleSolver {
 public:
    QuordleSolver(const string& solverType, size_t numSolvers);
    void processResults(const vector<WordleGuess>& guesses);
    string makeGuess(size_t numGuess, buf_ptr wb);
 private:
    vector<unique_ptr<WordleSolver>> m_solvers;
    vector<size_t> m_numCandidatesList;
    pair<size_t, size_t> m_lowestCountAndIndex; // 0: size, 1: index
    size_t m_numSolvers;
    //size_t m_numSolved{0};
};
