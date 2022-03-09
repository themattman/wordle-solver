#include "wordle_trie.h"

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

WordleTrieNode::~WordleTrieNode() {
    m_wordleTrie->removeFromCandidates(children);
}

bool WordleTrie::insert(string word) {
    cout << "insert:" << word << endl;
    if (word.size() != LETTER_COUNT) {
        return false;
    }
    insertAtNode(word, m_root);
    return true;
}

void WordleTrie::insertAtNode(string remainingWord, WordleTrieNode* node) {
    if (remainingWord.size() == 0) {
        return;
    }

    WordleTrieNode* nextNode;
    if (letterExists(remainingWord[0], node, &nextNode)) {
        cout << "found:" << remainingWord[0] << endl;
    } else {
        cout << "not found:" << remainingWord[0] << endl;
        node->children.push_back(new WordleTrieNode(remainingWord[0], this));
        nextNode = node->children.back();
    }
    insertAtNode(remainingWord.substr(1, remainingWord.size()-1), nextNode);
}

bool WordleTrie::letterExists(char letter, WordleTrieNode* node, WordleTrieNode** match) {
    for (size_t i = 0; i < node->children.size(); i++) {
        if (node->children[i]->val == letter) {
            *match = node->children[i];
            return true;
        }
    }
    return false;
}

void WordleTrie::removeFromCandidates(const vector<WordleTrieNode*>& candidates) {
    /* for (auto& c : candidates) { */
    /*     if (m_candidates.find(c) != m_candidates.end()) { */
    /*         m_candidates.erase(); */
    /*     } */
    /* } */

    /* for (size_t i = 0; i < candidates.size(); i++) { */
    /* } */
}

void WordleTrie::removeAllOfLetter(char letter, WordleTrieNode* node) {
    // TODO: Remove each word in deleted vectors from solution set
    //       => Node dtor has handle to m_candidates and deletes self?
    vector<WordleTrieNode*>::iterator toRemoveIter;
    for (auto it = node->children.begin(); it != node->children.end(); it++) {
        if ((*it)->val == letter) {
            toRemoveIter = it;
        } else {
            removeAllOfLetter(letter, *it); //node->children[i]);
        }
    }
    node->children.erase(toRemoveIter);
}

void WordleTrie::removeSingleLetter(size_t letterPosition, char letter) {
}

void WordleTrie::removeSiblingsExcept(size_t letterPosition, char letter) {
}

