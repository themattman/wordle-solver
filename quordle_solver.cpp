#include "helpers.h"
#include "quordle_solver.h"
#include "wordle_buffer.h"
#include "wordlist_wordle_solver.h"

QuordleSolver::QuordleSolver(const string& solverType, size_t numSolvers)
    : m_numSolvers(numSolvers) {
    for (size_t i = 0; i < numSolvers; i++) {
        m_solvers.push_back(make_unique<TrieBasedWordleSolver>(i));
        m_numCandidatesList.push_back(m_solvers[i]->getNumCandidates());
        m_isInPlay.push_back(true);
        //cout << "i: " << i << " " << m_solvers.size() << " = " << m_solvers[i]->getNumCandidates() << endl;
    }
    m_initCandidates = m_solvers[0]->getNumCandidates();
    m_lowestCountAndIndex = {m_initCandidates, -1};
}

void QuordleSolver::processResults(const vector<WordleGuess>& guesses) {
    // Mark any solvers as finished if result is all green
    m_lowestCountAndIndex = {m_initCandidates, -1};
    for (size_t i = 0; i < m_numSolvers; i++) {
        if (guesses[i].results == CorrectWordleResult) {
            m_isInPlay[i] = false;
            cout << "ONE OF THE GAMES IS SOLVED!!" << i << endl;
        }
    }

    for (size_t i = 0; i < m_numSolvers; i++) {
        if (m_isInPlay[i]) {
            cout << "process [" << i << "] ";
            m_solvers[i]->processResult(guesses[i]);
            m_numCandidatesList[i] = m_solvers[i]->getNumCandidates();
            if (m_numCandidatesList[i] < m_lowestCountAndIndex.first) {
                m_lowestCountAndIndex = {m_numCandidatesList[i], i};
            }
        }
    }
    printRemainingCandidates();
}

void QuordleSolver::printRemainingCandidates() const {
    for (size_t i = 0; i < m_numSolvers; i++) {
        cout << "solver[" << i << "]: " << m_numCandidatesList[i] << endl;
    }
    cout << "*lowest* sz:" << m_lowestCountAndIndex.first << " idx:" << m_lowestCountAndIndex.second << endl;
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
        for (size_t i = 0; i < m_numSolvers; i++) {
            if (m_isInPlay[i]) {
                cout << "Getting guess from: [" << i << "] w/numCandidates="
                   << m_solvers[i]->getNumCandidates() << endl;
                nextGuess = m_solvers[i]->makeSubsequentGuess(numGuess, wb, i);
            }
        }
    }

    // for (size_t i = 0; i < m_numSolvers; i++) {
    //     if (m_solvers[i]
    // }
    cout << "I pick: [" << nextGuess << "]" << endl;
    return nextGuess;
}
