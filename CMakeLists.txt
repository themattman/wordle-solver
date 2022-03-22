cmake_minimum_required (VERSION 2.6)
project(wordle-solver)
# set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-std=c++17 -g -ferror-limit=50") #-H")
set(CMAKE_EXE_LINKER_FLAGS "-v")

add_executable(solver wordle_selectors.cpp wordlist_wordle_solver.cpp wordle_trie.cpp wordle_checker.cpp main.cpp)

add_executable(test_solver wordle_checker_test.cpp wordle_checker.cpp wordle_selectors.cpp)
