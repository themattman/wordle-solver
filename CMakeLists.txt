cmake_minimum_required (VERSION 2.6)
project(wordle-solver)
# set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-std=c++17 -g") #-H")
set(CMAKE_EXE_LINKER_FLAGS "-v")

add_executable(solver wordle_selectors.cpp wordlist_wordle_solver.cpp wordle_trie.cpp wordle_checker.cpp main.cpp)

add_executable(test_solver wordle_checker_test.cpp wordle_checker.cpp wordle_selectors.cpp)

# "/Library/Developer/CommandLineTools/usr/bin/ld"
# -demangle
# -lto_library /Library/Developer/CommandLineTools/usr/lib/libLTO.dylib
# -dynamic
# -arch x86_64
# -platform_version macos 12.0.0 12.1
# -syslibroot /Library/Developer/CommandLineTools/SDKs/MacOSX12.1.sdk
# -o solver
# -search_paths_first
# -headerpad_max_install_names CMakeFiles/solver.dir/wordle_selectors.cpp.o CMakeFiles/solver.dir/wordlist_wordle_solver.cpp.o CMakeFiles/solver.dir/wordle_trie.cpp.o CMakeFiles/solver.dir/wordle_checker.cpp.o CMakeFiles/solver.dir/main.cpp.o
# -lc++
# -lSystem /Library/Developer/CommandLineTools/usr/lib/clang/13.1.6/lib/darwin/libclang_rt.osx.a
