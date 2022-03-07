#include "wordle_checker.h"
#include "wordle_rules.h"

#include <iostream>

using namespace std;


int main() {
    auto wc = WordleChecker{};
    wc.setAnswer("quiet");

    auto wg1 = WordleGuess{"alive"};
    if (!wc.check(wg1)) {
        cerr << "failure" << endl;
    }
    if (wg1.results != vector<WordleResult>{WordleResult::BLACK, WordleResult::BLACK, WordleResult::GREEN, WordleResult::BLACK, WordleResult::YELLOW}) {
        cerr << "failure" << endl;
    }

    auto wg2 = WordleGuess{"queen"};
    if (!wc.check(wg2)) {
        cerr << "failure" << endl;
    }
    if (wg2.results != vector<WordleResult>{WordleResult::GREEN, WordleResult::GREEN, WordleResult::YELLOW, WordleResult::BLACK, WordleResult::BLACK}) { // which e gets the color??
        cerr << "failure" << endl;
    }


    auto wg3 = WordleGuess{"moles"};
    if (!wc.check(wg3)) {
        cerr << "failure" << endl;
    }
    if (wg3.results != vector<WordleResult>{WordleResult::BLACK, WordleResult::BLACK, WordleResult::BLACK, WordleResult::GREEN, WordleResult::BLACK}) {
        cerr << "failure" << endl;
    }

    auto wg4 = WordleGuess{"zzzzz"};
    if (wc.check(wg4)) {
        cerr << "failure" << endl;
    }

    auto wg5 = WordleGuess{"slams"};
    if (!wc.check(wg5)) {
        cerr << "failure" << endl;
    }
    if (wg5.results != vector<WordleResult>{WordleResult::BLACK, WordleResult::BLACK, WordleResult::BLACK, WordleResult::BLACK, WordleResult::BLACK}) {
        cerr << "failure" << endl;
    }
}
