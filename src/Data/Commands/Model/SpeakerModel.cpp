#include "SpeakerModel.hpp"
#include "LexiconModel.hpp"
#include "../../../StackOverflow.hpp"
#include "../../../Program.hpp"

#include <iostream>
#include <cassert>
#include <cstring>

using namespace ScriptAI;
using namespace Sql;

SpeakerModel::SpeakerModel(size_t capacity, LexiconModel *lexicon): 
    speaker_ids(capacity), lexicon(lexicon) {
    assert(capacity > 0);
}

void SpeakerModel::clear() {
    this->speaker_ids.clear();
}

bool SpeakerModel::is_empty() {
    return this->speaker_ids.empty();
}

void SpeakerModel::add_speaker(size_t id) {
    if (this->speaker_ids.size() < this->speaker_ids.capacity()) {
        this->speaker_ids.push_back(id);
    } else {
        std::cerr << "Speaker overflow" << std::endl;
        assert(false);
    }
}

void SpeakerModel::add_speaker(std::string name) {
    trim(name);
    size_t id;
    if (this->lexicon->try_get_id(name, id)) {
        this->add_speaker(id);
    }
}

void SpeakerModel::copy(SpeakerModel* other) {
    assert(this->speaker_ids.capacity() == other->speaker_ids.capacity());
    this->speaker_ids = other->speaker_ids;
}