#include "wordle_buffer.h"
#include "wordle_rules.h"
#include "wordle_solver.h"

#include <memory>
#include <string>

//using namespace std;

bool runOneGame(const std::string& solverType, const std::string& answer, size_t idx, std::shared_ptr<WordleBuffer> wb);
void runAllWordsMultiThreaded(const std::string& solverType);
void runAllWords(const std::string& solverType);
void runDebug(std::unique_ptr<WordleSolverImpl> solver, const std::string& answer);
int interactiveMode(std::unique_ptr<WordleSolverImpl> solver);
int cheatMode(std::unique_ptr<WordleSolverImpl> solver);

