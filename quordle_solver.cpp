#include "helpers.h"
#include "quordle_solver.h"
#include "wordle_buffer.h"
#include "wordlist_wordle_solver.h"

QuordleSolver::QuordleSolver(const string& solverType, size_t numSolvers)
    : m_numSolvers(numSolvers) {
    for (size_t i = 0; i < numSolvers; i++) {
        m_solvers.push_back(make_unique<TrieBasedWordleSolver>(i));
        //m_solvers.push_back(helpers::createWordleSolver(solverType, i)); // Why??
        cout << "i: " << i << " " << m_solvers.size() << endl;
        m_solvers[i]->getNumCandidates();
        // auto wb = make_shared<WordleBuffer>();
        // m_solvers[i]->makeInitialGuess(wb, 0);
    }
}

void QuordleSolver::processResults(const vector<WordleGuess>& guesses) {
    for (size_t i = 0; i < m_numSolvers; i++) {
        cout << "process [" << i << "]";
        m_solvers[i]->processResult(guesses[i]);
        m_numCandidatesList[i] = m_solvers[i]->getNumCandidates();
        if (m_numCandidatesList[i] < m_lowestCountAndIndex.first) {
            m_lowestCountAndIndex = {m_numCandidatesList[i], i};
        }
    }
}

string QuordleSolver::makeGuess(size_t numGuess, buf_ptr wb) {
    string nextGuess;
    if (m_lowestCountAndIndex.first < 10) {
        // See if there's an obvious choice
        auto& solver = m_solvers[m_lowestCountAndIndex.second];
        nextGuess = solver->makeSubsequentGuess(numGuess, wb, m_lowestCountAndIndex.second);
    } else {
        // Choose best from all
        nextGuess = "empty";
    }
    for (size_t i = 0; i < m_numSolvers; i++) {
        
    }
    cout << "I pick: [" << nextGuess << "]" << endl;
    return nextGuess;
}
