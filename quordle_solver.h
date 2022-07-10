#include "wordle_rules.h"
#include "wordle_solver.h"

#include <string>
#include <vector>

using namespace std;

class QuordleSolver {
 public:
    QuordleSolver(const string& solverType, size_t numSolvers);
    void processResults(const vector<WordleGuess>& guesses);
 private:
    vector<unique_ptr<WordleSolver>> m_solvers;
    size_t m_numSolvers;
};
