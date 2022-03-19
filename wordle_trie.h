#pragma once

#include "wordle_solver.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <set>
#include <vector>

using namespace std;

class WordleTrie;

class WordleTrieNode {
public:
    WordleTrieNode() = delete;
    WordleTrieNode(char v, WordleTrie* wt, string prefix="", bool isLeaf=false)
        : val(v), children(), m_wordleTrie(wt), m_prefix(prefix), m_isLeaf(isLeaf) {}
    char val;
    vector<WordleTrieNode> children;
    bool operator==(const WordleTrieNode& node) const { return node.val == val; }
    bool operator!=(const WordleTrieNode& node) const { return node.val != val; }
private:
    friend WordleTrie;
    WordleTrie* m_wordleTrie;
    WordleTrieNode* m_parent;
    bool m_isLeaf;
    string m_prefix;
};

class WordleTrie {
public:
    WordleTrie() { m_root = new WordleTrieNode('_', this); }
    bool insert(string word);
    void fixupGreen(size_t letterPosition, char letter) {
        removeExceptLetterAtLevel(0, letterPosition, letter, *m_root);
    }
    void fixupYellow(size_t letterPosition, char letter, const string& guessStr) {
        if (countOccurs(letter, guessStr) == 1) {
            removeSingleLetter(letterPosition, letter);
        } else {
            removeSingleLetter(letterPosition, letter);
            //removeSingleLetter(0, letterPosition, letter, *m_root); ?
        }
    }
    void fixupBlack(size_t letterPosition, char letter, const string& guessStr) {
        if (countOccurs(letter, guessStr) == 1) {
            removeAllOfLetter(letter, *m_root);
        } else {
            removeSingleLetter(letterPosition, letter);
        }
    }
    string getCandidate(Selector* selector);
    size_t getNumCandidates() const { return m_candidates.size(); }
    void printCandidates();
private:
    size_t countOccurs(char letter, const string& word) { return std::count(word.begin(), word.end(), letter); }
    void insertAtNode(string prefix, string remainingWord, WordleTrieNode* node);
    bool letterExists(char letter, WordleTrieNode* node, WordleTrieNode** match);
    void addAllOfLetterToSolution(char letter);
    void addAllOfLetterToSolution(char letter, WordleTrieNode& node, vector<string>& letterCandidates);
    void removeFromCandidates(WordleTrieNode* node);
    void removeExceptLetterAtLevel(size_t curDepth, size_t letterPosition, char letter, WordleTrieNode& node);
    void removeLetterAtLevel(char letter, WordleTrieNode& node);
    void removeAllOfLetter(char letter, WordleTrieNode& node);
    void removeSingleLetter(size_t letterPosition, char letter);
    void removeSingleLetter(size_t curDepth, size_t letterPosition, char letter, WordleTrieNode& node);
    void removeAllChildren(WordleTrieNode& node);
    void addAllChildren(WordleTrieNode& node, vector<string>& letterCandidates);
    void promoteLetter();

    WordleTrieNode* getChild(size_t letterPosition, char letter);

    WordleTrieNode* m_root;
    set<string> m_candidates;
    vector<vector<string>> m_workingSets;
};
