#pragma once

#include "wordle_rules.h"
#include "wordle_solver.h"

#include <ctype.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


inline size_t countOccurs(char letter, const string& word) {
    return std::count(word.begin(), word.end(), letter);
}

class Helpers {
public:
    static WordleGuess promptUserToCheckGuess(const string& output, size_t guessNumber) {
        cout << "Guess #" << guessNumber << ": " << output << endl;

        string input = collectUserInput(/*isRepeat=*/false, /*isCheckingGuess=*/true);
        while(!isUserInputValidAnswer(input)) {
            input = collectUserInput(/*isRepeat=*/true, /*isCheckingGuess=*/true);
        }

        return stringToWordleGuess(output, input);
    }

    static string promptUserToMakeGuess(size_t guessNumber) {
        cout << "Guess #" << guessNumber << endl;

        string input = collectUserInput(/*isRepeat=*/false, /*isCheckingGuess=*/false);
        while(!isUserInputValidGuess(input)) {
            input = collectUserInput(/*isRepeat=*/true, /*isCheckingGuess=*/false);
        }

        return input;
    }

    static vector<string> getDictionary(const string& filename=DICTIONARY_FILENAME) {
        auto filein = ifstream(filename);
        vector<string> words;
        string word;
        while (std::getline(filein, word)) { words.push_back(word); }
        return words;
    }
private:
    static WordleGuess stringToWordleGuess(const string& guess, const string& str) {
        vector<WordleResult> wr;

        for (auto& s : str) {
            switch(tolower(s)) {
            case 'g':
                wr.push_back(WordleResult::GREEN);
                break;
            case 'y':
                wr.push_back(WordleResult::YELLOW);
                break;
            case 'b':
                wr.push_back(WordleResult::BLACK);
                break;
            default:
                if (DEBUG) {
                    cerr << "Error: [helpers] invalid wordle answer" << endl;
                }
                throw;
            }
        }

        return WordleGuess(guess, wr);
    }

    static string collectUserInput(bool isRepeat, bool isCheckingGuess) {
        if (isRepeat) {
            cout << "Invalid response. Try again." << endl;
        }

        string prompt = (isCheckingGuess) ? "checking" : "guessing";
        cout << prompt << "> ";
        string userInput;
        cin >> userInput;
        return userInput;
    }

    static bool isUserInputValidGuess(string& userInput) {
        if (userInput.size() != LETTER_COUNT) {
            return false;
        }

        for (size_t i = 0; i < userInput.size(); i++) {
            userInput[i] = tolower(userInput[i]);
            if (!isalpha(userInput[i])) {
                return false;
            }
        }

        return true;
    }

    static bool isUserInputValidAnswer(const string& userInput) {
        if (userInput.size() != LETTER_COUNT) {
            return false;
        }

        for (auto& s_any_case : userInput) {
            char s = tolower(s_any_case);
            if (!(s == 'g' || s == 'y' || s == 'b')) {
                return false;
            }
        }

        return true;
    }
};
