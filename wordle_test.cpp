#include "wordle_selector.h"
#include "wordle_trie.h"
#include <iostream>

using namespace std;

void printNodes(const WordleTrieNode& w, const WordleTrieNode& c) {
    cout << "--print-nodes--" << endl;
    cout << "      val = [" << w.val << "] => cp.val = [" << c.val << "]" << endl;
    cout << "#children = [" << w.children.size() << "] => cp.children = [" << c.children.size() << "]" << endl;
    for (size_t i = 0; i < w.children.size(); i++) {
        cout << "         - child = [" << w.children[i].val << "]";
        if (i < c.children.size()) {
            cout <<  " => cp.children = [" << c.children[i].val << "]" << endl;
        } else {
            cout << endl;
        }
    }
    cout << "     trie = [" << w.m_wordleTrie->getNumCandidates() << "] => cp.trie = ["
       << c.m_wordleTrie->getNumCandidates() << "]" << endl;
    cout << "   prefix = [" << w.m_prefix << "] => cp.prefix = [" << c.m_prefix << "]" << endl;
    cout << "   isLeaf = [" << w.m_isLeaf << "] => cp.isLeaf = [" << c.m_isLeaf << "]" << endl;
}

int main() {
    cout << "Copy Ctor" << endl;
    auto wt = WordleTrie();
    auto wtn = WordleTrieNode('c', &wt, nullptr, "cur", true);
    auto wtn2 = WordleTrieNode('d', &wt, nullptr, "great", true);
    auto wtn3 = WordleTrieNode('e', &wt, nullptr, "broke", true);
    wtn.children.push_back(wtn2);
    wtn2.children.push_back(wtn3);

    auto wtn_cp = wtn;
    printNodes(wtn, wtn_cp);

    cout << "\nMove Ctor" << endl;
    auto n = WordleTrieNode('z', nullptr, nullptr, "ok");
    n = move(wtn);

    printNodes(wtn, n);
    //cout << "n:" << endl
}
