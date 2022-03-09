#include "wordle_trie.h"

#include <iostream>
#include <iterator>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;


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
    m_candidates.insert(word);
    return true;
}

void WordleTrie::insertAtNode(string prefix, string remainingWord, WordleTrieNode* node) {
    if (remainingWord.size() == 0) {
        return;
    }

    WordleTrieNode* nextNode;
    if (!letterExists(remainingWord[0], node, &nextNode)) {
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
        auto nodeIter = m_candidates.find(node->m_prefix+node->val);
        if (nodeIter != m_candidates.end()) {
            m_candidates.erase(nodeIter);
        }
    }
}

void WordleTrie::removeAllOfLetter(char letter, WordleTrieNode& node) {
    // Base Case: At a leaf that can be deleted
    if (node.m_isLeaf && node.val == letter) {
        removeFromCandidates(&node);
        return;
    }

    removeLetterAtLevel(letter, node);
 
    // Remove letter from all other children
    for (auto it = node.children.begin(); it != node.children.end(); it++) {
        removeAllOfLetter(letter, *it);
    }
}

void WordleTrie::removeExceptLetterAtLevel(size_t curDepth, size_t letterPosition, char letter, WordleTrieNode& node) {
    if (curDepth == letterPosition) {
        for (size_t i = 0; i < node.children.size(); i++) {
            if (node.children[i].val != letter) {
                removeAllChildren(node.children[i]);
            }
        }
        return;
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        removeExceptLetterAtLevel(curDepth + 1, letterPosition, letter, node);
    }
}

void WordleTrie::removeLetterAtLevel(char letter, WordleTrieNode& node) {
   // Remove letter and its children at current level, if it exists
    for (auto it = node.children.begin(); it != node.children.end(); it++) {
        if (it->val == letter) {
            removeAllChildren(*it);
            break;
        }
    }
}

void WordleTrie::printCandidates() {
    cerr << "~~print~~" << endl;
    for (auto& c : m_candidates) {
        cerr << c << endl;
    }
    cerr << "~~~~~~" << endl;
}

void WordleTrie::addAllOfLetterToSolution(char letter) {
    vector<string> letterCandidates;
    addAllOfLetterToSolution(letter, *m_root, letterCandidates);
    m_workingSets.push_back(letterCandidates);
}

void WordleTrie::addAllOfLetterToSolution(char letter, WordleTrieNode& node, vector<string>& letterCandidates) {
    if (node.val == letter) {
        addAllChildren(node, letterCandidates);
        return;
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        if (node.children[i].val == letter) {
            addAllChildren(node, letterCandidates);
        } else {
            addAllOfLetterToSolution(letter, node.children[i], letterCandidates);
        }
    }
}

void WordleTrie::addAllChildren(WordleTrieNode& node, vector<string>& letterCandidates) {
    if (node.m_isLeaf) {
        letterCandidates.push_back(node.m_prefix + node.val);
        return;
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        addAllChildren(node.children[i], letterCandidates);
    }
}

void WordleTrie::removeSingleLetter(size_t letterPosition, char letter) {
    removeSingleLetter(0, letterPosition, letter, *m_root);
}

void WordleTrie::removeSingleLetter(size_t curDepth, size_t letterPosition, char letter, WordleTrieNode& node) {
    if (curDepth == letterPosition) {
        removeLetterAtLevel(letter, node);
        return;
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        removeSingleLetter(curDepth + 1, letterPosition, letter, node.children[i]);
    }
}

void WordleTrie::removeAllChildren(WordleTrieNode& node) {
    if (node.m_isLeaf) {
        removeFromCandidates(&node);
        return;
    }

    for (auto it = node.children.begin(); it != node.children.end(); it++) {
        removeAllChildren(*it);
    }
}
