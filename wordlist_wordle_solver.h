#pragma once

#include "wordle_solver.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
//#include <unordered_set>
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
                    m_letterMaps[i][curChar].push_back(word);
                } else {
                    m_letterMaps[i][curChar] = {word};
                }
            }
        }
        cout << "Size of wordlist: " << m_wordlist.size() << endl;
    }

    vector<string> m_wordlist;
    set<string> m_wordSet;
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
            string candidateWord = *(m_wordSet.begin());
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
    RandomPlusWordleSolver() : RandomWordleSolver() {
        for (size_t i = 0; i < LETTER_COUNT; i++) {
            m_workingSets.push_back(set<string>());
        }
    }

    void processResult(const WordleGuess& guess) override {
        // most restrictive -> least restrictive
        cout << "numSetWords before:" << m_wordSet.size() << endl;
        trimGreens(guess, createPositionVector(guess.results, WordleResult::GREEN));
        cout << "numSetWords green done:" << m_wordSet.size() << endl;
        trimBlacks(guess, createPositionVector(guess.results, WordleResult::BLACK));
        cout << "numSetWords black done:" << m_wordSet.size() << endl;
        trimYellows(guess, createPositionVector(guess.results, WordleResult::YELLOW));
        cout << "numSetWords yellow done:" << m_wordSet.size() << endl;

        // fix up wordSet
        m_wordSet.clear();

        vector<string> first;
        vector<string> result;
        for(size_t i = 0; i < LETTER_COUNT-1; i++) {
            if (first.size() == 0) {
                copy(m_workingSets[i].begin(), m_workingSets[i].end(), back_inserter(first));
                m_workingSets[i].clear();
                sort(first.begin(), first.end());
                if (first.size() == 0) {
                    continue;
                }
            }

            vector<string> second;
            copy(m_workingSets[i+1].begin(), m_workingSets[i+1].end(), back_inserter(second));
            m_workingSets[i+1].clear();
            sort(second.begin(), second.end());

            if (second.size() > 0) {
                set_intersection(first.begin(), first.end(), second.begin(), second.end(), result.begin());
                first = result;
                result.clear();
            }
        }
        m_workingSets[LETTER_COUNT-1].clear();

        cout << "numSetWords done done:" << m_wordSet.size() << endl;
        for (auto& w : m_wordSet) {
            cout << " w:" << w << endl;
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
        for (auto& p : positions) {
            includeInSet(g.guessStr[p], p);
        }
    }

    void trimYellows(WordleGuess g, const vector<size_t>& positions) {
        for (auto& p : positions) {
            for (size_t i = 0; i < LETTER_COUNT; i++) {
                if (i == p) {
                    excludeFromSet(g.guessStr[p], i);
                } else {
                    includeInSet(g.guessStr[p], i);
                }
            }
        }
    }

    void trimBlacks(WordleGuess g, const vector<size_t>& positions) {
        for (auto& p : positions) {
            for (size_t i = 0; i < LETTER_COUNT; i++) {
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

    void includeInSet(char includeChar, size_t letterPosition) {
        for (auto it = m_letterMaps[letterPosition][includeChar].begin(); it != m_letterMaps[letterPosition][includeChar].end(); it++) {
            m_workingSets[letterPosition].insert(*it);
        }

        cout << "letter: [" << includeChar << "]" << endl;
        for (auto it = m_letterMaps[letterPosition].begin(); it != m_letterMaps[letterPosition].end(); it++) {
            if (it->first != includeChar) {
                cout << " del:" << it->first << endl;
                it->second.clear();
            }
        }
    }

    vector<set<string>> m_workingSets;
};
