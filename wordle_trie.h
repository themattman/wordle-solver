#pragma once

#include "wordle_selector.h"
#include "wordle_solver.h"

#include <string>
#include <set>
#include <vector>

using namespace std;


class WordleTrie;

class WordleTrieNode {
public:
    WordleTrieNode() = delete;
    WordleTrieNode(char v, WordleTrie* wt, WordleTrieNode* parent, string prefix="", bool isLeaf=false)
        : val(v), children(), m_wordleTrie(wt), m_parent(parent), m_prefix(prefix), m_isLeaf(isLeaf) {}
    char val;
    vector<WordleTrieNode> children;
    bool operator==(const WordleTrieNode& node) const { return node.val == val; }
    bool operator!=(const WordleTrieNode& node) const { return node.val != val; }
private:
    friend WordleTrie;
    WordleTrie* m_wordleTrie;
    WordleTrieNode* m_parent;
    string m_prefix;
    bool m_isLeaf;
};

class WordleTrie {
public:
    WordleTrie() { m_root = new WordleTrieNode('_', this, nullptr); }
    bool insert(string word);
    void fixupGreen(size_t letterPosition, char letter) {
        removeExceptLetterAtLevel(0, letterPosition, letter, *m_root);
    }
    void fixupYellow(size_t letterPosition, char letter) {
        removeSingleLetter(letterPosition, letter);
        removeWordsWithoutLetter(letter);
    }
    void fixupBlack(size_t letterPosition, char letter) {
        removeAllOfLetter(letter, *m_root);
    }
    string getCandidate(unique_ptr<WordleSelector<SetIterator>>& selector, const vector<WordleKnown>& knowns, size_t guessNum);
    size_t getNumCandidates() const { return m_candidates.size(); }
    void printCandidates() const;
private:
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
    void removeWordsWithoutLetter(char letter);
    void removeWordsWithoutLetterAtLevel(size_t curDepth, char letter, WordleTrieNode& node);
    void removeWord(WordleTrieNode& node);
    WordleTrieNode* getChild(size_t letterPosition, char letter); // TODO: unused

    WordleTrieNode* m_root;
    set<string> m_candidates;
};
