cmake_minimum_required (VERSION 2.6)
project(wordle-solver)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-std=c++20 -Werror -Wall -stdlib=libc++")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")

# Configure different types of builds:
# $ cmake -DCMAKE_BUILD_TYPE=Release .
# $ cmake -DCMAKE_BUILD_TYPE=Debug .
#
# $ make

# Boost support for program_options (argument parsing)
#  - Linux: $ apt-get install boost
#  - MacOS: $ brew install boost
set(BOOST_BASE_DIR "/usr/local/Cellar/boost/1.78.0_1")
include_directories("${BOOST_BASE_DIR}/include")
find_library(LIB_LOCATION
    NAMES boost_program_options
    HINTS "${BOOST_BASE_DIR}/lib")

add_executable(solver wordle_selector.cpp wordlist_wordle_solver.cpp wordle_trie.cpp wordle_checker.cpp game_modes.cpp main.cpp)
target_link_libraries(solver ${LIB_LOCATION})

#add_executable(test_solver wordle_checker_test.cpp wordle_checker.cpp wordle_selector.cpp)
#add_executable(decoder wordle_decoder.cpp)
