#include "wordle_solver.h"

#include <iostream>
#include <string>
#include <unordered_set>
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
    bool operator==(const WordleTrieNode& node) const {
        //cout << "v:" << node.val << "==" << val << endl;
        return node.val == val; }
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
        removeSiblingsExcept(letterPosition, letter);
    }
    void fixupYellow(size_t letterPosition, char letter) {
        removeSingleLetter(letterPosition, letter);
        //promoteLetter(letter);
    }
    void fixupBlack(char letter) {
        removeAllOfLetter(letter, *m_root);
    }
    string getCandidate(size_t offset);
    size_t getNumCandidates() const { return m_candidates.size(); }
    void printCandidates();
private:
    void insertAtNode(string prefix, string remainingWord, WordleTrieNode* node);
    bool letterExists(char letter, WordleTrieNode* node, WordleTrieNode** match);
    void removeFromCandidates(WordleTrieNode* node);
    void removeLetterAtLevel(char letter, WordleTrieNode& node);
    void removeAllOfLetter(char letter, WordleTrieNode& node);
    void removeSingleLetter(size_t letterPosition, char letter);
    void removeSingleLetter(size_t curDepth, size_t letterPosition, char letter, WordleTrieNode& node);
    void removeAllChildren(WordleTrieNode& node);

    WordleTrieNode* getChild(size_t letterPosition, char letter);

    WordleTrieNode* m_root;
    unordered_set<string> m_candidates;
};
