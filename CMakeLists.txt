cmake_minimum_required (VERSION 2.6)
project(wordle-solver)
#set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-std=c++14 -g")

add_executable(solver main.cpp wordlist_wordle_solver.cpp wordle_trie.cpp wordle_checker.cpp)

