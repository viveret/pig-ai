#include "LexiconModel.hpp"
#include "SentenceModel.hpp"
#include "../../../StackOverflow.hpp"
#include "../../../SentenceIterator.hpp"

#include <iostream>
#include <cassert>
#include <random>

using namespace ScriptAI;
using namespace Sql;

LexiconModel::LexiconModel(int capacity) {
    if (capacity > 0) {
        this->lexicon.reserve(capacity);
    }
}

void LexiconModel::clear() {
    for (size_t i = 0; i < this->lexicon.size(); i++) {
        this->lexicon[i].feature_index = 0;
        this->lexicon[i].freq = 0;
        this->lexicon[i].occurrences = 0;
        this->lexicon[i].label = "";
    }
    this->lexicon.clear();
    this->_reverse.clear();
}

bool LexiconModel::is_empty() {
    return this->_reverse.empty();
}

void LexiconModel::add_sentence(SentenceModel *sentence) {
    for (FixedSentenceIterator it(sentence); it.good(); it.next()) {
        this->add_word(it.word);
    }
}

void LexiconModel::add_word(std::string& word) {
    trim(word);
    assert(word.length() > 0);
    auto e = this->_reverse.find(this->hash(word));
    if (e != this->_reverse.end()) {
        this->lexicon[e->second].occurrences++;
    } else {
        auto count = this->lexicon.size();
        this->lexicon.push_back(LexiconFeatureModel { count, 1, 0, word });
        this->_reverse[this->hash(word)] = count;
    }
}

void LexiconModel::add(LexiconFeatureModel* word) {
    if (word->feature_index >= this->lexicon.capacity()) {
        std::cerr << "Lexicon overflow" << std::endl;
    } else {
        this->lexicon.push_back(*word);
        this->_reverse[this->hash(word->label)] = word->feature_index;
        return;
    }
    
    assert(false);
}

void LexiconModel::copy(LexiconFeatureModel* word) {
    if (word->feature_index >= this->lexicon.size()) {
        std::cerr << "Lexicon overflow" << std::endl;
    } else {
        this->lexicon[word->feature_index].copy(word);
        this->_reverse[this->hash(word->label)] = word->feature_index;
        return;
    }
    
    assert(false);
}

bool LexiconModel::exists(std::string& word) {
    return this->_reverse.find(this->hash(word)) != this->_reverse.end();
}

bool LexiconModel::try_get_id(std::string& word, size_t &id) {
    auto e = this->_reverse.find(this->hash(word));
    if (e != this->_reverse.end()) {
        id = this->lexicon[e->second].feature_index;
        return true;
    }
    
    std::cerr << "No word " << word << " in lexicon" << std::endl;
    return false;
}

std::string LexiconModel::select_one(std::vector<double>& probs) {
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::discrete_distribution<size_t> distribution(probs.begin(),
                                                          probs.end());
    auto index = distribution(generator);
    if (index >= this->lexicon.capacity()) {
        std::cerr << "Lexicon overflow" << std::endl;
    } else if (index < this->lexicon.size()) {
        return this->lexicon[index].label;
    }
    assert(false);
    return "";
}

size_t LexiconModel::hash(std::string& word) {
    return std::hash<std::string>{}(word);
}