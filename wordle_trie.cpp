#include "wordle_trie.h"

#include <iostream>
#include <iterator>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

WordleTrieNode::~WordleTrieNode() {
    //cout << "dtor" << endl;
    // if (m_wordleTrie) {
    //     m_wordleTrie->removeFromCandidates(this);
    // }
}

string WordleTrie::getCandidate(size_t offset) {
    if (offset >= m_candidates.size()) {
        return "";
    }
    auto it = m_candidates.begin();
    advance(it, offset);
    return *it;
}

bool WordleTrie::insert(string word) {
    if (word.size() != LETTER_COUNT) {
        return false;
    }
    insertAtNode("", word, m_root);
    // cout << m_candidates.size() << " insert(" << word << ") ";
    m_candidates.insert(word);
    // cout << m_candidates.size() << endl;
    return true;
}

void WordleTrie::insertAtNode(string prefix, string remainingWord, WordleTrieNode* node) {
    if (remainingWord.size() == 0) {
        return;
    }

    WordleTrieNode* nextNode;
    if (!letterExists(remainingWord[0], node, &nextNode)) {
        //cout << "p: " << prefix << " v: " << remainingWord[0] << " isLeaf? " << (remainingWord.size() == 1) << endl;
        node->children.push_back(WordleTrieNode(remainingWord[0], nullptr, prefix, remainingWord.size() == 1));
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

void WordleTrie::removeFromCandidates(WordleTrieNode* node) {
    if (node->m_isLeaf) {
        // cout << "isLeaf: [" << node->m_prefix+node->val << "]" << endl;
        auto nodeIter = m_candidates.find(node->m_prefix+node->val);
        if (nodeIter != m_candidates.end()) {
            m_candidates.erase(nodeIter);
        }
    }
}

void WordleTrie::removeAllOfLetter(char letter, WordleTrieNode& node) {
    vector<WordleTrieNode>::iterator toRemoveIter;

    // Remove letter at current level, if it exists
    if (node.m_isLeaf && node.val == letter) {
        //cout << "Removing" << node.m_prefix << "|" << node.val << endl;
        removeFromCandidates(&node);
    } else {
        for (auto it = node.children.begin(); it != node.children.end(); it++) {
            if (it->val == letter) {
                toRemoveIter = it;
                // delete all children
                removeAllChildren(*it);
                removeFromCandidates(&(*toRemoveIter));
                break;
            }
        }

        // Remove letter from all children
        for (auto it = node.children.begin(); it != node.children.end(); it++) {
            //cout << "recurse" << endl;
            removeAllOfLetter(letter, *it);
        }
    }
}

void WordleTrie::printCandidates() {
    for (auto& c : m_candidates) {
        cout << c << endl;
    }
}

void WordleTrie::removeSingleLetter(size_t letterPosition, char letter) {
}

void WordleTrie::removeSiblingsExcept(size_t letterPosition, char letter) {
}

void WordleTrie::removeAllChildren(WordleTrieNode& node) {
    for (auto it = node.children.begin(); it != node.children.end(); it++) {
        if (it->m_isLeaf) {
            removeFromCandidates(&(*it));
        } else {
            removeAllChildren(*it);
        }
    }
}
