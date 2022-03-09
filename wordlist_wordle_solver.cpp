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

WordlistWordleSolver::WordlistWordleSolver(Selector* s) {
    *m_selector = *s;
    m_trie = new WordleTrie();
    loadWordList();
}

void WordlistWordleSolver::loadWordList() {
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        m_letterMaps.push_back(unordered_map<char, vector<string>>{});
    }
    // src: https://raw.githubusercontent.com/printfn/wordle-dict/main/answers.txt
    auto filein = ifstream("answers.txt");
    string word;
    while (std::getline(filein, word))
    {
        m_trie->insert(word);
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

string RandomWordleSolver::makeInitialGuess() {
    return m_selector->select(m_wordlist.begin(), m_wordlist.end());
    // // Put constraints like:
    // // - no double letter
    // // - distribution of common letters
    // if (m_wordSet.size() > 0) {
    //     size_t randomNumber = getRandomNumber();
    //     string candidateWord = *(m_wordSet.begin());
    //     while (containsDoubleLetter(candidateWord) && containsOneVowel(candidateWord)) {
    //         candidateWord = m_wordlist[randomNumber];
    //     }
    //     return candidateWord;
    // }

    // return "alive";
}

///////////////////

void NaiveRandomWordleSolver::processResult(const WordleGuess& guess) {
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

///////////////////

RandomPlusWordleSolver::RandomPlusWordleSolver() : RandomWordleSolver() {
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        m_workingSets.push_back(set<string>());
    }
}

void RandomPlusWordleSolver::processResult(const WordleGuess& guess) {
    // most restrictive -> least restrictive
    cout << "numSetWords before:" << m_wordSet.size() << endl;
    trimGreens(guess, createPositionVector(guess.results, WordleResult::GREEN));
    cout << "numSetWords green done:" << m_wordSet.size() << endl;
    trimYellows(guess, createPositionVector(guess.results, WordleResult::YELLOW));
    cout << "numSetWords yellow done:" << m_wordSet.size() << endl;

    // fix up wordSet
    m_wordSet.clear();
    auto results = processIntersectionOfIncludes();
    propagateIncludes(guess, results);
    cout << "numSetWords done done:" << m_wordSet.size() << endl;

    trimBlacks(guess, createPositionVector(guess.results, WordleResult::BLACK));
    cout << "numSetWords black done:" << m_wordSet.size() << endl;
}

void RandomPlusWordleSolver::propagateIncludes(const WordleGuess& wg, const vector<string>& refinedResult) {
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        if (wg.results[i] == WordleResult::GREEN) {
            char includeChar = wg.guessStr[i];
            m_letterMaps[i][includeChar] = refinedResult;
        }
    }
}

vector<string> RandomPlusWordleSolver::processIntersectionOfIncludes() {
    cout << "process-intersection-of-includes" << endl;
    vector<string> first;
    vector<string> result;
    copy(m_workingSets[0].begin(), m_workingSets[0].end(), back_inserter(first));
    m_workingSets[0].clear();
    sort(first.begin(), first.end());

    for(size_t i = 0; i < LETTER_COUNT-1; i++) {
        copy(m_workingSets[i].begin(), m_workingSets[i].end(), back_inserter(first));
        m_workingSets[i].clear();
        sort(first.begin(), first.end());
        if (first.size() == 0) {
            cout << "first is empty" << endl;
            continue;
        }

        vector<string> second;
        copy(m_workingSets[i+1].begin(), m_workingSets[i+1].end(), back_inserter(second));
        m_workingSets[i+1].clear();
        sort(second.begin(), second.end());

        if (second.size() > 0) {
            cout << "second isn't  empty" << endl;
            printVector(first, "f");
            printVector(second, "s");
            set_intersection(first.begin(), first.end(), second.begin(), second.end(), inserter(result, result.begin()));
            printVector(result, "r");
            first = result;
            result.clear();
        } else {
            cout << "second is empty" << endl;
        }
    }

    set<string> s(first.begin(), first.end());
    m_wordSet = move(s);

    return first;
}

void RandomPlusWordleSolver::printVector(const vector<string>& v, const string& s) const {
    cout << "vector " << s << " size:" << v.size() << endl;
    for (auto& w : v) {
        cout << " w:" << w << endl;
    }
}

vector<size_t> RandomPlusWordleSolver::createPositionVector(const vector<WordleResult>& allPositions, WordleResult wr) const {
    vector<size_t> positions;
    for (size_t i = 0; i < allPositions.size(); i++) {
        if (allPositions[i] == wr) {
            positions.push_back(i);
        }
    }
    return positions;
}

void RandomPlusWordleSolver::trimGreens(WordleGuess g, const vector<size_t>& positions) {
    for (auto& p : positions) {
        includeInSet(g.guessStr[p], p);
    }
}

void RandomPlusWordleSolver::trimYellows(WordleGuess g, const vector<size_t>& positions) {
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

void RandomPlusWordleSolver::trimBlacks(WordleGuess g, const vector<size_t>& positions) {
    for (auto& p : positions) {
        for (size_t i = 0; i < LETTER_COUNT; i++) {
            excludeFromSet(g.guessStr[p], i);
        }
    }
}

void RandomPlusWordleSolver::excludeFromSet(char excludeChar, size_t letterPosition) {
    for (auto it = m_letterMaps[letterPosition][excludeChar].begin(); it != m_letterMaps[letterPosition][excludeChar].end(); it++) {
        string& removeWord = *it;
        if (m_wordSet.find(removeWord) != m_wordSet.end()) {
            m_wordSet.erase(m_wordSet.find(removeWord));
        }
    }
    m_letterMaps[letterPosition][excludeChar].clear();
}

void RandomPlusWordleSolver::includeInSet(char includeChar, size_t letterPosition) {
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

/////////////////

string TrieBasedWordleSolver::makeInitialGuess() {
    return m_selector->select(m_wordlist.begin(), m_wordlist.end());

    // Put constraints like:
    // - no double letter
    // - distribution of common letters
    return "honey";
    if (m_trie->getNumCandidates() > 0) {
        string candidateWord = m_trie->getCandidate();
        if (candidateWord.size() == 0) {
            throw;
        }
        return candidateWord;
    }

    throw;
    //return "alive";
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
