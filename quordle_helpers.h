#pragma once

#include "common_helpers.h"
#include "quordle_rules.h"
#include "quordle_solver.h"

#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


namespace Helpers {

vector<WordleGuess> promptUserToCheckQuordleGuess(const string& output, size_t guessNumber) {
    cout << "   Guess #" << guessNumber << ": " << output << endl;
    vector<WordleGuess> guesses;
    for (size_t i = 0; i < NUM_QUORDLE_GAMES; i++) {
        string input = collectUserInput(/*isRepeat=*/false, ("#" + to_string(i) + " checking"));
        while(!isUserInputValidAnswer(input)) {
            input = collectUserInput(/*isRepeat=*/true, ("#" + to_string(i) + " checking"));
        }

        guesses.push_back(stringToWordleGuess(output, input));
    }
    return guesses;
}

} // namespace Helpers
