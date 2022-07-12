#pragma once

#include "wordle_buffer.h"
#include "wordle_solver.h"

#include <future>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace std;


int quordleCheatMode(const string& solverType);
bool runOneGame(const string& solverType, const string& answer, size_t idx, shared_ptr<WordleBuffer> wb);
void runAllWordsMultiThreaded(const string& solverType);
void runAllWords(const string& solverType);
void runDebug(unique_ptr<WordleSolverImpl> solver, const string& answer);
int interactiveMode(unique_ptr<WordleSolverImpl> solver);
int cheatMode(unique_ptr<WordleSolverImpl> solver);
