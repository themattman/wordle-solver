#include "wordle_trie.h"

#include <iterator>
#include <string>
#include <vector>

using namespace std;


string WordleTrie::getCandidate(Selector<SetIterator>* selector, const vector<WordleKnown>& knowns, size_t guessNum) {
    return selector->select(m_candidates.begin(), m_candidates.end(), m_candidates.size(), knowns, guessNum);
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
        node->children.push_back(WordleTrieNode(remainingWord[0], nullptr, node, prefix, remainingWord.size() == 1));
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
        removeExceptLetterAtLevel(curDepth + 1, letterPosition, letter, node.children[i]);
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

void WordleTrie::removeWordsWithoutLetter(char letter) {
    removeWordsWithoutLetterAtLevel(0, letter, *m_root);
}

void WordleTrie::removeWordsWithoutLetterAtLevel(size_t curDepth, char letter, WordleTrieNode& node) {
    if (node.m_isLeaf && node.val != letter) {
        removeWord(node);
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        if (node.children[i].val != letter) {
            removeWordsWithoutLetterAtLevel(curDepth + 1, letter, node.children[i]);
        }
    }
}

/**
 * Input: leaf node (i.e. a word)
 * Removes word from candidate list and (**future work**) then propagates the removal in the Trie
 *  by removing all parent nodes that have no children. Shrinks the trie appropriately.
 */
void WordleTrie::removeWord(WordleTrieNode& node) {
    if (!node.m_isLeaf) {
        if (DEBUG) cerr << "Error: [trie] removing non-leaf node" << endl;
        throw;
    }

    removeFromCandidates(&node);
}
