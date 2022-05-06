cmake_minimum_required (VERSION 2.6)
project(wordle-solver)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -O3 -stdlib=libc++")

# Boost support for program_options (argument parsing)
#  - Linux: $ apt-get install boost
#  - MacOS: $ brew install boost
set(BOOST_BASE_DIR "/usr/local/Cellar/boost/1.78.0_1")
include_directories("${BOOST_BASE_DIR}/include")
find_library(LIB_LOCATION
    NAMES boost_program_options
    HINTS "${BOOST_BASE_DIR}/lib")

add_executable(solver wordle_selectors.cpp wordlist_wordle_solver.cpp wordle_trie.cpp wordle_checker.cpp main.cpp)
target_link_libraries(solver ${LIB_LOCATION})

add_executable(test_solver wordle_checker_test.cpp wordle_checker.cpp wordle_selectors.cpp)
add_executable(decoder wordle_decoder.cpp)
