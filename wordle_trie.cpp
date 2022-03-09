#include "wordle_trie.h"

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

WordleTrieNode::~WordleTrieNode() {
    cout << "dtor" << endl;
    m_wordleTrie->removeFromCandidates(this);
}

bool WordleTrie::insert(string word) {
    if (word.size() != LETTER_COUNT) {
        return false;
    }
    insertAtNode("", word, m_root);
    return true;
}

void WordleTrie::insertAtNode(string prefix, string remainingWord, WordleTrieNode* node) {
    if (remainingWord.size() == 0) {
        return;
    }

    WordleTrieNode* nextNode;
    if (!letterExists(remainingWord[0], node, &nextNode)) {
        cout << "p: " << prefix << " v: " << remainingWord[0] << " isLeaf? " << (remainingWord.size() == 1) << endl;
        node->children.push_back(WordleTrieNode(remainingWord[0], this, prefix, remainingWord.size() == 1));
        nextNode = &node->children.back();
    }
    insertAtNode(prefix+remainingWord[0], remainingWord.substr(1, remainingWord.size()-1), nextNode);
}

bool WordleTrie::letterExists(char letter, WordleTrieNode* node, WordleTrieNode** match) {
    for (size_t i = 0; i < node->children.size(); i++) {
        if (node->children[i].val == letter) {
            *match = &(node->children[i]);
            return true;
        }
    }
    return false;
}

void WordleTrie::removeFromCandidates(WordleTrieNode* node) {//const vector<WordleTrieNode*>& candidates) {
    if (node->m_isLeaf) {
        cout << "isLeaf: [" << node->m_prefix << "]" << endl;
        auto nodeIter = m_candidates.find(node->m_prefix);
        if (nodeIter != m_candidates.end()) {
            m_candidates.erase(nodeIter);
        }
    // } else {
    //     for (auto& c : candidates) {
    //         removeFromCandidates();
    //         auto nodeIter = m_candidates.find(c->val);
    //         if (nodeIter != m_candidates.end()) {
    //             m_candidates.erase(nodeIter);
    //         }
    //     }
        return;
    }
    cout << "!isLeaf" << endl;
    /* for (size_t i = 0; i < candidates.size(); i++) { */
    /* } */
}

void WordleTrie::removeAllOfLetter(char letter, WordleTrieNode& node) {
    // TODO: Remove each word in deleted vectors from solution set
    //       => Node dtor has handle to m_candidates and deletes self?
    vector<WordleTrieNode>::iterator toRemoveIter;

    // Remove letter at current level, if it exists
    for (auto it = node.children.begin(); it != node.children.end(); it++) {
        if (it->val == letter) {
            //cout << "found! " << (*it)->m_prefix << " l:" << letter << " v:" << (*it)->val << endl;
            toRemoveIter = it;
            node.children.erase(toRemoveIter);
            break;
        }
    }

    // Remove letter from all children
    for (auto it = node.children.begin(); it != node.children.end(); it++) {
        //cout << "recurse" << endl;
        removeAllOfLetter(letter, *it);
    }
}

void WordleTrie::removeSingleLetter(size_t letterPosition, char letter) {
}

void WordleTrie::removeSiblingsExcept(size_t letterPosition, char letter) {
}

