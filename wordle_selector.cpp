#include "wordle_selector.h"
#include "wordle_helpers.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

using namespace std;

#define NUM_EASY_MODE_GUESSES 2


template <typename IterType>
string RandomWordleSelector<IterType>::select(IterType begin, IterType end, size_t rangeSize,
                                              const vector<WordleKnown>& knowns, size_t guessNum) {
    advance(begin, getRandom(begin, end, rangeSize));
    return *begin;
}

template <typename IterType>
size_t RandomWordleSelector<IterType>::getRandom(IterType begin, IterType end, size_t rangeSize) const {
    srand(time(NULL));
    return rand() % rangeSize;
}

template <typename IterType>
string EnhancedRandomWordleSelector<IterType>::select(IterType begin, IterType end, size_t rangeSize,
                                                      const vector<WordleKnown>& knowns, size_t guessNum) {
    string selection;
    do {
        selection = RandomWordleSelector<IterType>::select(begin, end, rangeSize, knowns, guessNum);
    } while (containsDoubleLetter(selection) && containsOneVowel(selection));
    return selection;
}

template <typename IterType>
bool EnhancedRandomWordleSelector<IterType>::containsDoubleLetter(const string& word) const {
    for (size_t i = 0; i < word.size(); i++) {
        for (size_t j = i+1; j < word.size(); j++) {
            if (word[i] == word[j]) {
                return true;
            }
        }
    }

    return false;
}

template <typename IterType>
bool EnhancedRandomWordleSelector<IterType>::isVowel(char letter) const {
    return (letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u' || letter == 'y');
}

template <typename IterType>
bool EnhancedRandomWordleSelector<IterType>::containsOneVowel(const string& word) const {
    size_t numVowels = 0;

    for (auto& letter : word) {
        numVowels = isVowel(letter) ? numVowels+1 : numVowels;
    }

    return numVowels <= 1;
}

////////////////

template <typename IterType>
string MostCommonLetterWordleSelector<IterType>::select(IterType begin, IterType end, size_t rangeSize,
                                                        const vector<WordleKnown>& knowns, size_t guessNum) {
    clearOldState();
    this->m_guessNum = guessNum;
    m_knowns = knowns;
    m_iterBegin = begin;
    m_iterEnd = end;
    computeFrequencyMap();
#if PRINT_GUESSES == true
    printCandidates();
#endif
    return getBestCandidate((guessNum <= NUM_EASY_MODE_GUESSES));
}

template <typename IterType>
void MostCommonLetterWordleSelector<IterType>::clearOldState() {
    m_knowns.clear();
    m_frequencyMapLetter.clear();
    m_wordScore.clear();
    m_sortedWords.clear();
}

template <typename IterType>
void MostCommonLetterWordleSelector<IterType>::printCandidates() const {
    cout << "Remaining Candidates: (" << m_sortedWords.size() << ")" << endl;
    size_t i = 0;
    for (auto it = m_sortedWords.begin(); it != m_sortedWords.end(); it++) {
        cout << " " << it->word << ":" << it->score << endl;
        i++;

        if (i == PRINT_GUESSES_SIZE) {
            cout << "... (" << (m_sortedWords.size() - PRINT_GUESSES_SIZE) << ") skipped" << endl;
            break;
        }
    }
}

template <typename IterType>
bool MostCommonLetterWordleSelector<IterType>::isInRange(const string& word) const {
    for (auto it = m_iterBegin; it != m_iterEnd; it++) {
        if (*it == word) {
            return true;
        }
    }
    return false;
}

template <typename IterType>
string MostCommonLetterWordleSelector<IterType>::getBestCandidate(bool isInOriginalDictionary) const {
    //return m_sortedWords.begin()->word;
    cout << "guess: [" << this->m_guessNum << "] isIn: [" << isInOriginalDictionary << "]" << endl;
    auto dict = Helpers::getDictionary();
    string topWord;
    auto wordIt = m_sortedWords.begin();
    if (!isInOriginalDictionary) {
        return wordIt->word;
    }

    size_t i = 0;
    do {
        topWord = wordIt->word;
        wordIt++;
        if (i < 10) {
            cout << "Selecting [" << topWord << "]" << endl;
            i++;
        }
    } while((!Helpers::isWordInDictionary(topWord, dict)) || (!isInRange(topWord)));
    return topWord;
}

template <typename IterType>
char MostCommonLetterWordleSelector<IterType>::getMostCommonLetter() const {
    size_t highest = 0;
    char mostCommon = '0';
    for (auto it = m_frequencyMapLetter.begin(); it != m_frequencyMapLetter.end(); it++) {
        if (it->second > highest) {
            highest = it->second;
            mostCommon = it->first;
        }
    }

    if (!mostCommon) {
        if (DEBUG) cerr << "Error: [selector] no most common letter" << endl;
        throw;
    }

    return mostCommon;
}

template <typename IterType>
void MostCommonLetterWordleSelector<IterType>::computeFrequencyMap() {
    if (m_alphabetWordScore.size() == 0) {
        computeFrequencyMapInternal(m_alphabetFrequencyMapLetter, m_alphabetWordScore);
        m_frequencyMapLetter = m_alphabetFrequencyMapLetter;
        m_wordScore = m_alphabetWordScore;
    } else {
        computeFrequencyMapInternal(m_frequencyMapLetter, m_wordScore);
    }
    sortWordsByFrequency();
}

template <typename IterType>
void MostCommonLetterWordleSelector<IterType>::sortWordsByFrequency() {
    for (auto wordIt = m_wordScore.begin(); wordIt != m_wordScore.end(); wordIt++) {
        auto ws = WordScore();
        ws.word = wordIt->first;
        ws.score = wordIt->second;
        m_sortedWords.insert(ws);
    }
}

template <typename IterType>
void NaiveMostCommonLetterWordleSelector<IterType>::computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                                                                unordered_map<string, size_t>& wordScore) {
    // Compute letter scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        for (auto& c : *wordIt) {
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
        }
    }

    // Compute word scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        for (auto& c : *wordIt) {
            if (wordLetters.find(c) == wordLetters.end()) {
                wordLetters.insert(c);
                score += letterMap[c];
            }
        }
        wordScore.insert({*wordIt, score});
    }
}

template <typename IterType>
void ImprovedMostCommonLetterWordleSelector<IterType>::computeFrequencyMapInternal(unordered_map<char, size_t>& letterMap,
                                                                                   unordered_map<string, size_t>& wordScore) {
    // Compute letter scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        for (auto& c : *wordIt) {
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
        }
    }

    // Compute word scores
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        set<char> greenLetters;
        for (size_t i = 0; i < this->m_knowns.size(); i++) {
            if (this->m_knowns[i].result == WordleResult::GREEN) {
                greenLetters.insert(this->m_knowns[i].letter);
            }
        }
        size_t i = 0;
        for (auto& c : *wordIt) {
            // only give scores to each letter once
            if (wordLetters.find(c) == wordLetters.end()) {
                // only give scores to each letter that isn't already known
                if (this->m_knowns[i].result != WordleResult::GREEN) {
                    if (greenLetters.find(c) == greenLetters.end()) {
                        score += letterMap[c];
                    }
                }
            }
            wordLetters.insert(c);
            i++;
        }
        wordScore.insert({*wordIt, score});
    }
}

template <typename IterType>
void PositionalLetterWordleSelector<IterType>::computeFrequencyMapInternal(unordered_map<char, size_t>& unused_letterMap,
                                                                           unordered_map<string, size_t>& wordScore) {
    // Compute positional letter scores
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        auto letterMap = unordered_map<char, size_t>();
        for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
            char c = (*wordIt)[i];
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
        }
        m_positionLetterScores.push_back(letterMap);
    }

    // Compute word scores & write to file
#if CREATE_SCORES_FILE == true
    ofstream file_stream;
    if (m_initialGuess && g_num_runs == 0) {
        file_stream.open(DICTIONARY_SCORES_FILENAME);
    }
#endif
    for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
        size_t score = 0;
        set<char> wordLetters;
        set<char> greenLetters;
        for (size_t i = 0; i < this->m_knowns.size(); i++) {
            if (this->m_knowns[i].result == WordleResult::GREEN) {
                greenLetters.insert(this->m_knowns[i].letter);
            }
        }
        size_t i = 0;
#if CREATE_SCORES_FILE == true
        if (m_initialGuess && g_num_runs == 0) {
            file_stream << *wordIt << ":";
        }
#endif
        for (auto& c : *wordIt) {
            // only give scores to each letter once // for the opening 2 rounds
            if ((wordLetters.find(c) == wordLetters.end() && this->m_guessNum < 6) || this->m_guessNum >= 6) {
                // only give scores to each letter that isn't already known
                //if (this->m_knowns[i].result != WordleResult::GREEN) {
                    if (greenLetters.find(c) == greenLetters.end()) {
                        score += m_positionLetterScores[i][c];
                    }
                    //}
            }
#if CREATE_SCORES_FILE == true
            if (m_initialGuess && g_num_runs == 0) {
                file_stream << m_positionLetterScores[i][c] << ",";
            }
#endif
            wordLetters.insert(c);
            i++;
        }
#if CREATE_SCORES_FILE == true
        if (m_initialGuess && g_num_runs == 0) {
            file_stream << " = " << score << endl;
        }
#endif
        wordScore.insert({*wordIt, score});
    }

    m_initialGuess = false;
}

template <typename IterType>
void PositionalLetterWordleSelector<IterType>::clearOldState() {
    MostCommonLetterWordleSelector<IterType>::clearOldState();
    m_positionLetterScores.clear();
}

template <typename IterType>
void FrequencyAndPositionalLetterWordleSelector<IterType>::computeFrequencyMapInternal(unordered_map<char, size_t>& unused_letterMap,
                                                                                       unordered_map<string, size_t>& wordScore) {
    // Compute positional letter scores from range
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        auto letterMap = unordered_map<char, size_t>();
        for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
            char c = (*wordIt)[i];
            if (letterMap.find(c) == letterMap.end()) {
                letterMap[c] = 0;
            }
            letterMap[c]++;
        }
        m_positionLetterScores.push_back(letterMap);
    }

    // Compute letter scores from range
    for (size_t i = 0; i < LETTER_COUNT; i++) {
        for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
            char c = (*wordIt)[i];
            if (m_fullLetterMap.find(c) == m_fullLetterMap.end()) {
                m_fullLetterMap[c] = 0;
            }
            m_fullLetterMap[c]++;
        }
    }

#if CREATE_SCORES_FILE == true
    ofstream file_stream;
    if (m_initialGuess && g_num_runs == 0) {
        file_stream.open(DICTIONARY_SCORES_FILENAME);
    }
#endif

    // IterType itBegin;
    // IterType itEnd;
    // unordered_map<string, size_t>::iterator itBegin;
    // unordered_map<string, size_t>::iterator itEnd;
    if (this->m_guessNum <= NUM_EASY_MODE_GUESSES) {
        // Create range from full guess file
        ifstream full_guess_file_stream;
        string word;
        full_guess_file_stream.open(DICTIONARY_GUESSES_FILENAME);
        while (std::getline(full_guess_file_stream, word)) {
            wordScore[word] = 0;
        }
        auto itBegin = wordScore.begin();
        auto itEnd = wordScore.end();

        for (auto wordIt = itBegin; wordIt != itEnd; wordIt++) {
            size_t score = 0;
            set<char> wordLetters;
            set<char> greenLetters;
            for (size_t i = 0; i < this->m_knowns.size(); i++) {
                if (this->m_knowns[i].result == WordleResult::GREEN) {
                    greenLetters.insert(this->m_knowns[i].letter);
                }
            }
            size_t i = 0;
#if CREATE_SCORES_FILE == true
            if (m_initialGuess && g_num_runs == 0) {
                file_stream << wordIt->first << ":";
            }
#endif
            for (auto& c : wordIt->first) {
                // only give scores to each letter once
                if ((wordLetters.find(c) == wordLetters.end() && this->m_guessNum < 6) || this->m_guessNum >= 6) {
                    if (greenLetters.find(c) == greenLetters.end()) {
                        score += (m_positionLetterScores[i][c] + 2*m_fullLetterMap[c]);
                    }
                }
#if CREATE_SCORES_FILE == true
                if (m_initialGuess && g_num_runs == 0) {
                    file_stream << (m_positionLetterScores[i][c] + 2*m_fullLetterMap[c]) << ",";
                }
#endif
                wordLetters.insert(c);
                i++;
            }
#if CREATE_SCORES_FILE == true
            if (m_initialGuess && g_num_runs == 0) {
                file_stream << " = " << score << endl;
            }
#endif
            wordScore[wordIt->first] = score;
        }
    } else {
        for (auto wordIt = this->m_iterBegin; wordIt != this->m_iterEnd; wordIt++) {
            size_t score = 0;
            set<char> wordLetters;
            set<char> greenLetters;
            for (size_t i = 0; i < this->m_knowns.size(); i++) {
                if (this->m_knowns[i].result == WordleResult::GREEN) {
                    greenLetters.insert(this->m_knowns[i].letter);
                }
            }
            size_t i = 0;
#if CREATE_SCORES_FILE == true
            if (m_initialGuess && g_num_runs == 0) {
                file_stream << wordIt << ":";
            }
#endif
            for (auto& c : *wordIt) {
                // only give scores to each letter once
                if ((wordLetters.find(c) == wordLetters.end() && this->m_guessNum < 6) || this->m_guessNum >= 6) {
                    if (greenLetters.find(c) == greenLetters.end()) {
                        score += (m_positionLetterScores[i][c] + 2*m_fullLetterMap[c]);
                    }
                }
#if CREATE_SCORES_FILE == true
                if (m_initialGuess && g_num_runs == 0) {
                    file_stream << (m_positionLetterScores[i][c] + 2*m_fullLetterMap[c]) << ",";
                }
#endif
                wordLetters.insert(c);
                i++;
            }
#if CREATE_SCORES_FILE == true
            if (m_initialGuess && g_num_runs == 0) {
                file_stream << " = " << score << endl;
            }
#endif
            wordScore[*wordIt] = score;
        }
    }

    m_initialGuess = false;
}
