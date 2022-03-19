cmake_minimum_required (VERSION 2.6)
project(wordle-solver)
#set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-std=c++14 -g") # -H")

add_executable(solver main.cpp wordlist_wordle_solver.cpp wordle_trie.cpp wordle_checker.cpp wordle_selectors.cpp)

add_executable(test_solver wordle_checker_test.cpp wordle_checker.cpp wordle_selectors.cpp)

