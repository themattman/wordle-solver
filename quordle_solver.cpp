#include "quordle_solver.h"
#include "wordle_buffer.h"
#include "wordle_helpers.h"

QuordleSolver::QuordleSolver(const string& solverType, size_t numSolvers)
    : m_numSolvers(numSolvers) {
    for (size_t i = 0; i < numSolvers; i++) {
        m_solvers.emplace_back(make_unique<TrieBasedWordleSolver>(i));
        //m_solvers.push_back(Helpers::createWordleSolver(solverType, i)); // Why??
        cout << "i: " << i << " " << m_solvers.size() << endl;
        m_solvers[i]->getNumCandidates();
        // auto wb = make_shared<WordleBuffer>();
        // m_solvers[i]->makeInitialGuess(wb, 0);
    }
}

void QuordleSolver::processResults(const vector<WordleGuess>& guesses) {
    for (size_t i = 0; i < m_numSolvers; i++) {
        cout << "process [" << i << "]" << endl;
        m_solvers[i]->processResult(guesses[i]);
    }
}
