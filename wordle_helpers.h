#pragma once

#include "wordle_solver.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;


class Helpers {
public:
    static WordleGuess promptUser(const string& output, size_t guessNumber) {
        cout << "Guess #" << guessNumber << ": " << output << endl;

        string input = collectUserInput(/*isRepeat=*/false);
        while(!isUserInputValid(input)) {
            input = collectUserInput(/*isRepeat=*/true);
        }

        return stringToWordleGuess(output, input);
    }
private:
    static WordleGuess stringToWordleGuess(const string& guess, const string& str) {
        vector<WordleResult> wr;

        for (auto& s : str) {
            switch(s) {
            case 'G':
                wr.push_back(WordleResult::GREEN);
                break;
            case 'Y':
                wr.push_back(WordleResult::YELLOW);
                break;
            case 'B':
                wr.push_back(WordleResult::BLACK);
                break;
            default:
                throw;
            }
        }

        return WordleGuess(guess, wr);
    }

    static string collectUserInput(bool isRepeat) {
        if (isRepeat) {
            cout << "Invalid response. Try again." << endl;
        }
        cout << "> ";
        string userInput;
        cin >> userInput;
        return userInput;
    }

    static bool isUserInputValid(const string& userInput) {
        if (userInput.size() != LETTER_COUNT) {
            return false;
        }

        for (auto& s : userInput) {
            if (!(s == 'G' || s == 'Y' || s == 'B')) {
                return false;
            }
        }

        return true;
    }
};
