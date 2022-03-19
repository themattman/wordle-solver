#include "wordle_checker.h"
#include "wordle_rules.h"

#include <iostream>

using namespace std;


int main() {
    auto wc = WordleChecker{};
    wc.setAnswer("quiet");

    auto wg1 = WordleGuess{"alive"};
    size_t numGuesses = 0;
    if (!wc.check(wg1, numGuesses)) {
        cerr << "failure" << endl;
    }
    if (wg1.results != vector<WordleResult>{WordleResult::BLACK, WordleResult::BLACK, WordleResult::GREEN, WordleResult::BLACK, WordleResult::YELLOW}) {
        cerr << "failure" << endl;
    }

    auto wg2 = WordleGuess{"queen"};
    if (!wc.check(wg2, numGuesses)) {
        cerr << "failure" << endl;
    }
    if (wg2.results != vector<WordleResult>{WordleResult::GREEN, WordleResult::GREEN, WordleResult::YELLOW, WordleResult::BLACK, WordleResult::BLACK}) { // which e gets the color??
        cerr << "failure" << endl;
    }

    auto wg3 = WordleGuess{"moles"};
    if (wc.check(wg3, numGuesses)) {
        cerr << "failure" << endl;
    }
    if (wg3.results != vector<WordleResult>{WordleResult::BLACK, WordleResult::BLACK, WordleResult::BLACK, WordleResult::GREEN, WordleResult::BLACK}) {
        cerr << "failure" << endl;
    }

    auto wg4 = WordleGuess{"zzzzz"};
    if (wc.check(wg4, numGuesses)) {
        cerr << "failure" << endl;
    }

    auto wg5 = WordleGuess{"slams"};
    if (wc.check(wg5, numGuesses)) {
        cerr << "failure" << endl;
    }
    if (wg5.results != vector<WordleResult>{WordleResult::BLACK, WordleResult::BLACK, WordleResult::BLACK, WordleResult::BLACK, WordleResult::BLACK}) {
        cerr << "failure" << endl;
    }
}


/*
a: haute
g: harsh => se
   GGBBB

a: sweet
g: leeen => what is the first e?
   BBGGB => greens take precedence
   BYGBB => ordering important
   BYGYB (probably not)


a: board
g: agape
   ?BGBB
g: drird

what does this mean for my algo?


can i make a new selector strategy?
RandomSelector<iterable>

answer: haute
 guess: verve
        BBBBG
 guess: veest
        BYBBY

^^ this tells me that you must handle greens first for elimination... but also need letter frequency before being overaggressive for blacks
store all locations of same letter. if only letter and black, then eliminate all.
otherwise, 

if freq > 1
 yellow = eliminate current
 black = eliminate current


Black:
if letterFreqInGuess == 1
 removeAll
else if letterFreqInGuess == 2 && 1 == G:
 removeAllExcept(depth)
else if letterFreqInGuess == 2 && 1 == Y:
 
 */
