#include "quordle_solver.h"
#include "wordle_helpers.h"

QuordleSolver::QuordleSolver(const string& solverType, size_t numSolvers)
    : m_numSolvers(numSolvers) {
    for (size_t i = 0; i < numSolvers; i++) {
        m_solvers.emplace_back(Helpers::createWordleSolver(solverType, i));
    }
}

void QuordleSolver::processResults(const vector<WordleGuess>& guesses) {
    for (size_t i = 0; i < m_numSolvers; i++) {
        m_solvers[i]->processResult(guesses[i]);
    }
}
