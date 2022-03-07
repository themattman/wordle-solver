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
        if (m_wordSet.size() > 0) {
            size_t randomNumber = getRandomNumber();
            //string candidateWord = m_wordlist[randomNumber];
            string candidateWord = *(m_wordSet.begin()); // + randomNumber);
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
        /* random_device rd;  // obtain a random number from hardware */
        /* mt19937 gen(rd()); // seed the generator */
        /* uniform_int_distribution<> distr(0, m_wordlist.size()); // define the range */
        /* return distr(gen); */
        return rand() % m_wordlist.size();
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
        if (guess != CorrectWordleGuess) {
            // most restrictive -> least restrictive
            cout << "numSetWords before:" << m_wordSet.size() << endl;
            trimGreens(guess, createPositionVector(guess.results, WordleResult::GREEN));
            cout << "numSetWords green done:" << m_wordSet.size() << endl;
            trimBlacks(guess, createPositionVector(guess.results, WordleResult::BLACK));
            cout << "numSetWords black done:" << m_wordSet.size() << endl;
            trimYellows(guess, createPositionVector(guess.results, WordleResult::YELLOW));
            cout << "numSetWords yellow done:" << m_wordSet.size() << endl;
        }
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
        m_wordSet.clear();
        for (auto& p : positions) {
            char curGreenChar = g.guessStr[p];
            for (auto it = m_letterMaps[p][curGreenChar].begin(); it != m_letterMaps[p][curGreenChar].end(); it++) {
                m_wordSet.insert(*it);
            }
        }
    }

    void trimYellows(WordleGuess g, const vector<size_t>& positions) {
        for (auto& p : positions) {
            char curYellowChar = g.guessStr[p];
            for (size_t i = 0; i < LETTER_COUNT; i++) {
                if (i != p) {
                    // include in final answer
                } else {
                    //exclude
                    excludeFromSet(g.guessStr[p], i);
                    /* for (auto it = m_letterMaps[i][curYellowChar].begin(); it != m_letterMaps[i][curYellowChar].end(); it++) { */
                    /*     string& removeWord = *it; */
                    /*     if (m_wordSet.find(removeWord) != m_wordSet.end()) { */
                    /*         m_wordSet.erase(m_wordSet.find(removeWord)); */
                    /*     } */
                    /* } */
                    /* m_letterMaps[i][curYellowChar].clear(); */
                }
            }
        }
    }

    void trimBlacks(WordleGuess g, const vector<size_t>& positions) {
        for (auto& p : positions) {
            for (size_t i = 0; i < LETTER_COUNT; i++) {
                // delete curBlackChar element from all maps, iterate over all elements and remove from master set
                excludeFromSet(g.guessStr[p], i);
            }
        }
    }

    void excludeFromSet(char excludeChar, size_t letterPosition) {
        for (auto it = m_letterMaps[letterPosition][excludeChar].begin(); it != m_letterMaps[letterPosition][excludeChar].end(); it++) {
            string& removeWord = *it;
            if (m_wordSet.find(removeWord) != m_wordSet.end()) {
                m_wordSet.erase(m_wordSet.find(removeWord));
            }
        }
        m_letterMaps[letterPosition][excludeChar].clear();
    }

    void includeInSet() {
    }
};
