#include "wordlist_wordle_solver.h"
#include "wordle_helpers.h"
#include "wordle_rules.h"
#include "wordle_selector.h"
#include "wordle_solver.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


WordlistWordleSolver::WordlistWordleSolver() : WordleSolverImpl() {
    m_selector = WordleSelectorFactory<SetIterator>::makeWordleSelector(WordleSelectorType::FrequencyAndPositionalLetter); // Choose `WordleSelector` HERE!!
    loadWordList([this](const string& word) -> void {
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
    });
}

void WordlistWordleSolver::loadWordList(function<void(string)> eachLineCallback) {
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        m_letterMaps.push_back(unordered_map<char, vector<string>>{});
    }

    // src: https://raw.githubusercontent.com/printfn/wordle-dict/main/answers.txt
    // src: https://raw.githubusercontent.com/AllValley/WordleDictionary/main/wordle_complete_dictionary.txt
    auto filein = ifstream(DICTIONARY_FILENAME);
    string word;
    while (std::getline(filein, word)) {
        eachLineCallback(word);
    }

    if (DEBUG) cout << "Size of wordlist: " << m_wordlist.size() << endl;
}

/////////////////////

TrieBasedWordleSolver::TrieBasedWordleSolver() : PassthroughWordleSolver() {
    m_selector = WordleSelectorFactory<SetIterator>::makeWordleSelector(WordleSelectorType::FrequencyAndPositionalLetter); // Choose `WordleSelector` HERE!!
    m_trie = new WordleTrie();
    loadWordList([this](const string& line){
        m_trie->insert(line);
    });
}

string TrieBasedWordleSolver::makeInitialGuess() {
    if (m_trie->getNumCandidates() > 0) {
        string candidateWord = "slice"; //m_trie->getCandidate(m_selector, m_knownCorrects, 0);
        if (candidateWord.size() == 0) {
            if (DEBUG) cerr << "Error: [solver] empty word" << endl;
            throw;
        }
        cerr << m_trie->getNumCandidates() << ",";
        return candidateWord;
    }

    if (DEBUG) cerr << "Error: [solver] no more candidates" << endl;
    throw;
}

string TrieBasedWordleSolver::makeSubsequentGuess(size_t numGuess) {
    if (m_trie->getNumCandidates() > 0) {
        string candidateWord = m_trie->getCandidate(m_selector, m_knownCorrects, numGuess);
        if (candidateWord.size() == 0) {
            if (DEBUG) cerr << "Error: [solver] empty word" << endl;
            throw;
        }
        cerr << m_trie->getNumCandidates() << ",";
        return candidateWord;
    }

    if (DEBUG) cerr << "Error: [solver] no more candidates" << endl;
    throw;
}

void TrieBasedWordleSolver::printNumCands(const string& color) const {
    if (DEBUG) {
        string color_code;
        string end_color_code = "\033[0m";
        if (color == "green") {
            color_code = "\033[0;32m";
        } else if (color == "yellow") {
            color_code = "\033[0;33m";
        } else if (color == "black") {
            color_code = "\033[0;30m";
        }
        cout << "numCandidates [" << color_code << color << end_color_code << "] done:" << m_trie->getNumCandidates() << endl;
    }
}

void TrieBasedWordleSolver::processResult(const WordleGuess& guess) {
    // most restrictive -> least restrictive
    trimGreens(guess, createPositionVector(guess, WordleResult::GREEN));
    printNumCands("green");
    trimYellows(guess, createPositionVector(guess, WordleResult::YELLOW));
    printNumCands("yellow");
    trimBlacks(guess, createPositionVector(guess, WordleResult::BLACK));
    printNumCands("black");
#if PRINT_GUESSES == true
    m_trie->getCandidate(m_selector, m_knownCorrects, 0);
#endif
}

vector<size_t> TrieBasedWordleSolver::createPositionVector(const WordleGuess& allPositions, WordleResult wr) {
    vector<size_t> positions;
    for (size_t i = 0; i < allPositions.results.size(); i++) {
        if (allPositions.results[i] == wr) {
            positions.push_back(i);
            if (wr == WordleResult::GREEN && m_knownCorrects[i].result != WordleResult::GREEN) {
                m_knownCorrects[i].result = WordleResult::GREEN;
                m_knownCorrects[i].letter = allPositions.guessStr[i];
            }
        }
    }

    return positions;
}

void TrieBasedWordleSolver::trimGreens(const WordleGuess& g, const vector<size_t>& positions) {
    for (auto& p : positions) {
        m_trie->fixupGreen(p, g.guessStr[p]);
    }
}

void TrieBasedWordleSolver::trimYellows(const WordleGuess& g, const vector<size_t>& positions) {
    for (auto& p : positions) {
        m_trie->fixupYellow(p, g.guessStr[p]);
    }
}

void TrieBasedWordleSolver::trimBlacks(const WordleGuess& g, const vector<size_t>& positions) {
    for (auto& p : positions) {
        if (countOccurs(g.guessStr[p], g.guessStr) > 1) {
            if (isAnotherOccurrenceNotBlack(p, g)) {
                // only remove letter for current slot
                m_trie->fixupYellow(p, g.guessStr[p]);
            } else {
                // remove letter from all slots
                m_trie->fixupBlack(p, g.guessStr[p]);
            }
        } else {
            m_trie->fixupBlack(p, g.guessStr[p]);
        }
    }
}

bool TrieBasedWordleSolver::isAnotherOccurrenceNotBlack(size_t position, const WordleGuess& g) const {
    char letter = g.guessStr[position];
    for (size_t i = 0; i < g.guessStr.size(); i++) {
        if (i == position) continue;
        if (g.guessStr[i] == letter) {
            if (g.results[i] != WordleResult::BLACK) {
                return true;
            }
        }
    }

    return false;
}
