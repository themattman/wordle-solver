/*
 * Date: 03/06/2022
 * Author: Matt Kneiser
 * Purpose: Solve Wordle
 */

/*
$ ./solver [strategy selection]
Spit out random word
Take user input for correct/incorrect letters via B/Y/G
Echo best guess
<repeat>
 */

#include "wordle_solver.h"
#include "wordlist_wordle_solver.h"
#include "wordle_helpers.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;


int main() {
    //auto solver = RandomPlusWordleSolver();
    auto solver = TrieBasedWordleSolver();
    //return 0;
    size_t numGuesses = 1;
    WordleGuess wg = Helpers::promptUser(solver.makeInitialGuess(), numGuesses);
    if (wg != CorrectWordleGuess) {
        solver.processResult(wg);
        for (numGuesses++; numGuesses <= MAX_GUESSES; numGuesses++) {
            wg = Helpers::promptUser(solver.makeInitialGuess(), numGuesses);
            if (wg == CorrectWordleGuess) {
                break;
            }
            solver.processResult(wg);
        }
    }

    if (numGuesses >= MAX_GUESSES && wg != CorrectWordleGuess) {
        cout << "Darn!" << endl;
    } else {
        cout << "Hell yeah!" << endl;
        cout << "Wordle " << numGuesses << "/" << MAX_GUESSES << endl;
    }

    return 0;
}
