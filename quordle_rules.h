#pragma once

#include <string>
#include <vector>

#define NUM_QUORDLE_GAMES 4

static const size_t QUORDLE_MAX_GUESSES = 10;

// struct QuordleGuess {
//     QuordleGuess(std::string cGuess, std::vector<std::vector<WordleResult> > cResults)
//         : guessStr(cGuess), results(cResults) {}
//     QuordleGuess(std::string cGuess)
//         : guessStr(cGuess) {
//         auto results = std::vector<std::vector<WordleResult> >();
//     }
//     // inline bool operator==(const WordleGuess& w1) const {
//     //     return w1.results == results;
//     // }
//     // inline bool operator!=(const WordleGuess& w1) const {
//     //     return w1.results != results;
//     // }

//     std::string guessStr;
//     std::vector<std::vector<WordleResult> > results;
// };
