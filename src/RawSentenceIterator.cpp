#include "SentenceIterator.hpp"
#include "Data/Commands/Model/SentenceModel.hpp"
#include "StackOverflow.hpp"
#include "Data/Regex.hpp"
#include "queue.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>

using namespace ScriptAI;
using namespace Sql;

std::regex_token_iterator<std::string::iterator> end;

// https://en.cppreference.com/w/cpp/string/byte/toupper
std::string& str_toupper(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                // static_cast<int(*)(int)>(std::toupper)         // wrong
                // [](int c){ return std::toupper(c); }           // wrong
                // [](char c){ return std::toupper(c); }          // wrong
                   [](unsigned char c){ return std::toupper(c); } // correct
                  );
    return s;
}

RawSentenceIterator::RawSentenceIterator(std::string line):
    SentenceIterator(), _line(line), part_index(0),
    part_iterator(_line.begin(), _line.end(), rgx_import_sentence()) {
    str_toupper(_line);
    while (part_iterator != end && (&*part_iterator) != nullptr) {
        if (assignment_acceptable()) {
            return;
        } else {
            part_iterator++;
        }
    }
    this->part_iterator = end;
}

RawSentenceIterator::~RawSentenceIterator() {
    if (!this->words.empty()) {
        this->words.clear();
    }
}

void RawSentenceIterator::next() {
    if (words.empty()) {
        while (part_iterator++ != end && (&*part_iterator) != nullptr) {
            if (assignment_acceptable()) {
                index++;
                return;
            }
        }
        this->part_iterator = end;
        return;
    } else {
        this->word = this->words.back();
        this->words.pop_back();
        return;
    }
}

bool RawSentenceIterator::assignment_acceptable() {
    auto prefixes = list_import_word_prefix();
    this->part = part_iterator->str();
    trim(part, ' ');
    part_index++;
    if (part.length() > 0) {
        auto nofixes = list_import_word_nofix();
        auto part_in_nofix = std::find(nofixes.begin(), nofixes.end(), part);
        if (std::regex_match(part, rgx_import_word_ignore())) {
            return false;
        } else if (part.length() > 4) {
            if (part_in_nofix == nofixes.end()) {
                std::string before(part), prefix_found, suffix_found;
                for (auto it = prefixes.begin(); it != prefixes.end(); it++) {
                    if (part.compare(0, it->size(), *it) == 0) {
                        part.erase(0, it->size());
                        prefix_found = *it;
                        break;
                    }
                }

                auto suffixes = list_import_word_suffix();
                for (auto it = suffixes.begin(); it != suffixes.end(); it++) {
                    int start = std::max(0, (int) (part.size() - it->size())), len = std::min(it->size(), part.size() - start);
                    if (part.compare(start, len, *it) == 0) {
                        part = part.substr(0, part.size() - it->size());
                        suffix_found = *it;
                        break;
                    }
                }

                if (!prefix_found.empty() || !suffix_found.empty()) {
                    if (!suffix_found.empty()) {
                        this->words.push_back(suffix_found);
                    }
                    if (!part.empty()) {
                        this->words.push_back(part);
                    }
                    if (!prefix_found.empty()) {
                        this->words.push_back(prefix_found);
                    }

                    this->word = this->words.back();
                    this->words.pop_back();
                    return true;
                }
            }
        }

        this->word = part;
        return true;
    }
    return false;
}

bool RawSentenceIterator::good() { return part_iterator != end && this->word.length() > 0; }