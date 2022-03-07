#pragma once

#include "wordle_solver.h"

#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class WordlistWordleSolver : public WordleSolver {
public:
    WordlistWordleSolver() {
        loadWordList();
    }

protected:
    void loadWordList() {
        for (size_t i = 0; i < LETTER_COUNT; i++) {
            m_letterMaps.push_back(unordered_map<char, vector<string>>{});
        }
        // src: https://raw.githubusercontent.com/printfn/wordle-dict/main/answers.txt
        auto filein = ifstream("answers.txt");
        string word;
        while (std::getline(filein, word))
        {
            m_wordlist.push_back(word);
            m_wordSet.insert(word);
            for (size_t i = 0; i < LETTER_COUNT; i++) {
                char curChar = word[i];
                if (m_letterMaps[i].find(curChar) != m_letterMaps[i].end()) {
                    /* if (i == 2 && curChar == 'a') { */
                    /*     cout << " w:" << word << endl; */
                    /* } */
                    m_letterMaps[i][curChar].push_back(word);
                    /* if (i == 2 && curChar == 'a') { */
                    /*     cout << "--------" << endl; */
                    /*     for (auto it = m_letterMaps[i][curChar].begin(); it != m_letterMaps[i][curChar].end(); it++) { */
                    /*         cout << " wi:" << *it << endl; */
                    /*     } */
                    /*     cout << "========" << endl; */
                    /* } */
                } else {
                    m_letterMaps[i][curChar] = {word};
                }
            }
        }
        cout << "Size of wordlist: " << m_wordlist.size() << endl;
    }

    vector<string> m_wordlist;
    unordered_set<string> m_wordSet;
    vector<
        unordered_map<char, vector<string>>
        > m_letterMaps;
};


/////////////////////

class RandomWordleSolver : public WordlistWordleSolver {
public:
    RandomWordleSolver() : WordlistWordleSolver() {}

    string makeInitialGuess() {
        // Put constraints like:
        // - no double letter
        // - distribution of common letters
        if (m_wordlist.size() > 0) {
            size_t randomNumber = getRandomNumber();
            string candidateWord = m_wordlist[randomNumber];
            while (containsDoubleLetter(candidateWord) && containsOneVowel(candidateWord)) {
                cout << "candidateWord not accepted:" << candidateWord << endl;
                candidateWord = m_wordlist[randomNumber];
            }
            cout << "candidateWord accepted:" << candidateWord << endl;
            return candidateWord;
        }

        return "alive";
    }

    string makeSubsequentGuess() {
        return makeInitialGuess();
    }
protected:
    size_t getRandomNumber() {
        random_device rd;  // obtain a random number from hardware
        mt19937 gen(rd()); // seed the generator
        uniform_int_distribution<> distr(0, m_wordlist.size()); // define the range
        return distr(gen);
    }

    bool containsDoubleLetter(string word) {
        for (size_t i = 0; i < word.size(); i++) {
            for (size_t j = i+1; j < word.size(); j++) {
                if (word[i] == word[j]) {
                    return true;
                }
            }
        }

        return false;
    }

    bool isVowel(char letter) {
        return (letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u' || letter == 'y');
    }

    bool containsOneVowel(string word) {
        size_t numVowels = 0;

        for (auto& letter : word) {
            numVowels = isVowel(letter) ? numVowels+1 : numVowels;
        }

        return numVowels > 1;
    }
};

///////////////////

class NaiveRandomWordleSolver : public RandomWordleSolver {
public:
    NaiveRandomWordleSolver() : RandomWordleSolver() {}

    void processResult(const WordleGuess& guess) {
        if (guess != CorrectWordleGuess) {

            // Naive erase
            for (size_t i = 0; i < m_wordlist.size(); i++) {
                if (m_wordlist[i] == guess.guessStr) {
                    m_wordlist.erase(m_wordlist.begin() + i);
                    break;
                }
            }

            cout << "Size of wordlist: " << m_wordlist.size() << endl;
        }
    }
};

///////////////////
class RandomPlusWordleSolver : public RandomWordleSolver {
public:
    RandomPlusWordleSolver() : RandomWordleSolver() {}

    void processResult(const WordleGuess& guess) override {
        trimBlacks(guess, createPositionVector(guess.results, WordleResult::BLACK));
        trimGreens(guess, createPositionVector(guess.results, WordleResult::GREEN));
        trimYellows(guess, createPositionVector(guess.results, WordleResult::YELLOW));
    }
protected:
    vector<size_t> createPositionVector(const vector<WordleResult>& allPositions, WordleResult wr) {
        vector<size_t> positions;
        for (size_t i = 0; i < allPositions.size(); i++) {
            if (allPositions[i] == wr) {
                positions.push_back(i);
            }
        }
        return positions;
    }

    void trimGreens(WordleGuess g, const vector<size_t>& positions) {
        for (auto& p : positions) {
            char curGreenChar = g.guessStr[p];
            cout << "numGreens: " << m_letterMaps[p][curGreenChar].size() << endl;
        }
        //find_if(m_wordlist.begin(), m_wordlist.end(), matchesPosition);
    }

    void trimYellows(WordleGuess g, const vector<size_t>& positions) {
        for (auto& p : positions) {
            char curYellowChar = g.guessStr[p];
            cout << "round [" << p << "]" << endl;
            size_t sum = 0;
            for (size_t i = 0; i < LETTER_COUNT; i++) {
                if (i != p) {
                    cout << " numYellows: " << m_letterMaps[i][curYellowChar].size() << endl;
                    sum += m_letterMaps[i][curYellowChar].size();
                }
            }
            cout << " sum: " << sum << endl;
            //cout << "numYellows: " << m_letterMaps[p][curYellowChar].size() << endl;
        }
    }

    void trimBlacks(WordleGuess g, const vector<size_t>& positions) {
        for (auto& p : positions) {
            char curBlackChar = g.guessStr[p];
            cout << "round [" << p << "]" << endl;
            size_t sum = 0;
            for (size_t i = 0; i < LETTER_COUNT; i++) {
                // delete curBlackChar element from all maps, iterate over all elements and remove from master set
                cout << " numBlacks: " << m_letterMaps[i][curBlackChar].size() << endl;
                sum += m_letterMaps[i][curBlackChar].size();
                for (auto it = m_letterMaps[i][curBlackChar].begin(); it != m_letterMaps[i][curBlackChar].end(); i++) {
                    cout << " =>" << *it << endl;
                    //string& removeWord = *it;
                    /* if (m_wordSet.find(removeWord) != m_wordSet.end()) { */
                    /*     m_wordSet.erase(m_wordSet.find(removeWord)); */
                    /* } else { */
                    /*     cout << "noexist:" << removeWord << endl; */
                    /* } */
                }
                //m_letterMaps[i][curBlackChar].clear();
            }
            cout << " sum: " << sum << endl;
        }
        /* for (auto& p : positions) { */
        /*     vector<vector<>::iterator> deleteIdxs; */
        /*     char incorrectLetter = g.guessStr[p]; */
        /*     for (size_t i = 0; i < m_wordlist.size(); i++) { */
        /*         if (m_wordlist[i][p] == incorrectLetter) { */
        /*             deleteIdxs.push_back(i); */
        /*         } */
        /*     } */

        /*     for (size_t i = 0; i < deleteIdxs.size(); i++) { */
        /*     } */
        /* } */
    }

    /* unordered_set<string> m_wordSet; */
};

/*


set< >

existence of letters in spot
5 different slot maps.
[a]: <>
[b]: <>

set_union for yellows
deletes for blacks
set_intersection for greens










 */
