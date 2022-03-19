#include "wordlist_wordle_solver.h"
#include "wordle_rules.h"
#include "wordle_solver.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


/**

Solution:


[A]                                [M]                                [A]               [Z]               [E]
 |                                  |                                  |                 |                 |
[a]->["abort","apart",...,"azure"] [a]->["bacon","badge",...,"badly"]
[b]->["bacon","basis",...,"bylaw"] [b]->["aback","abase",...,"obese"]
[d]->["daily","dingy",...,"dimes"] [c]->["acorn","acrid",...,"scuba"]
[e]->["earns","eater",...,"enums"] [d]->["adage","adapt",...,"udder"]
 .                                  .
[m]->["mails","mates",...,"moles"] [m]->["amass","amaze",...,"umbra"]
 .                                  .
 .                                  .
[z]->["zebra","zings",...,"zooms"] [z]->["azure","ozone"]

# GREEN
Select row. Remove all remainders from the 1st dim.

# YELLOW
Remove row. Select all other rows for letter in others.

# BLACK
Remove row from each dimension.

Removals leave structure in correct state.
Selects for yellow need to be unioned.
Selects for green need to be intersected.

Tree
                    (root)
                   /  | ..  \
                /     |   ..  \
             /        |    ..   \
1:          a         b     ..    z
          / | \     / | \       / | \
2:      b   c  z   a  e  y     e  i  o

 */

WordlistWordleSolver::WordlistWordleSolver(Selector* s) : WordleSolver(s) {
    cout << "WLWS" << endl;
    cout << "WLWS:" << s << "|" << m_selector << endl;
    loadWordList();
}

void WordlistWordleSolver::loadWordList() { // eachLineCallbackActor
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        m_letterMaps.push_back(unordered_map<char, vector<string>>{});
    }
    // src: https://raw.githubusercontent.com/printfn/wordle-dict/main/answers.txt
    auto filein = ifstream("answers.txt");
    string word;
    while (std::getline(filein, word))
    {
        // TODO: initialize trie in derived
        //m_trie->insert(word);
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
    // cout << "Size of candidates: " << m_trie->getNumCandidates() << endl;
    cout << "Size of wordlist: " << m_wordlist.size() << endl;
}

/////////////////////

string TrieBasedWordleSolver::makeInitialGuess() {
    if (m_trie->getNumCandidates() > 0) {
        string candidateWord = m_trie->getCandidate(m_selector);
        if (candidateWord.size() == 0) {
            throw;
        }
        return candidateWord;
    }

    throw;
}

string TrieBasedWordleSolver::makeSubsequentGuess() {
    return makeInitialGuess();
    // if (m_trie->getNumCandidates() > 0) {
    //     string candidateWord = m_trie->getCandidate();
    //     if (candidateWord.size() == 0) {
    //         throw;
    //     }
    //     return candidateWord;
    // }
    // throw;
}

void TrieBasedWordleSolver::processResult(const WordleGuess& guess) {
    // most restrictive -> least restrictive
    cout << "numCandidates before:" << m_trie->getNumCandidates() << endl;
    trimGreens(guess, createPositionVector(guess.results, WordleResult::GREEN));
    cout << "numCandidates green done:" << m_trie->getNumCandidates() << endl;
    trimYellows(guess, createPositionVector(guess.results, WordleResult::YELLOW));
    cout << "numCandidates yellow done:" << m_trie->getNumCandidates() << endl;
    trimBlacks(guess, createPositionVector(guess.results, WordleResult::BLACK));
    cout << "numCandidates black done:" << m_trie->getNumCandidates() << endl;
    m_trie->printCandidates();
}

vector<size_t> TrieBasedWordleSolver::createPositionVector(const vector<WordleResult>& allPositions, WordleResult wr) const {
    vector<size_t> positions;
    for (size_t i = 0; i < allPositions.size(); i++) {
        if (allPositions[i] == wr) {
            positions.push_back(i);
        }
    }
    return positions;
}

void TrieBasedWordleSolver::trimGreens(WordleGuess g, const vector<size_t>& positions) {
    for (auto& p : positions) {
        m_trie->fixupGreen(p, g.guessStr[p]);
    }
}

void TrieBasedWordleSolver::trimYellows(WordleGuess g, const vector<size_t>& positions) {
    for (auto& p : positions) {
        m_trie->fixupYellow(p, g.guessStr[p]);
    }
}

void TrieBasedWordleSolver::trimBlacks(WordleGuess g, const vector<size_t>& positions) {
    for (auto& p : positions) {
        cout << "black [" << p << "]" << endl;
        m_trie->fixupBlack(g.guessStr[p]);
    }
}
