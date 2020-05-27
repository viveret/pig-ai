#include "SentenceIterator.hpp"
#include "Data/Commands/Model/SentenceModel.hpp"

using namespace ScriptAI;
using namespace Sql;

FixedSentenceIterator::FixedSentenceIterator(const SentenceModel *sentence): SentenceIterator(), _doing_speaker(true), last_pos(0), tmp_pos(0), _sentence(sentence) {
    this->speaker_name_length = this->_sentence->speaker_name.length();
    this->what_said_length = this->_sentence->what_said.length();
    this->next();
    if (this->index > 0) {
        this->index = 0;
    }
}

FixedSentenceIterator::FixedSentenceIterator(const SentenceModel &sentence): SentenceIterator(), _doing_speaker(true), last_pos(0), _sentence(&sentence) {
    this->speaker_name_length = this->_sentence->speaker_name.length();
    this->what_said_length = this->_sentence->what_said.length();
    this->next();
    if (this->index > 0) {
        this->index = 0;
    }
}

void FixedSentenceIterator::next() {
    do {
        if (this->_doing_speaker) {
            this->tmp_pos = this->_sentence->speaker_name.find_first_of(' ', this->last_pos);
            if (this->tmp_pos > this->last_pos && this->tmp_pos < this->speaker_name_length) {
                this->word = this->_sentence->speaker_name.substr(this->last_pos, this->tmp_pos - this->last_pos);
            } else {
                this->word = this->_sentence->speaker_name.substr(this->last_pos, this->speaker_name_length - this->last_pos);
                this->_doing_speaker = false;
                this->last_pos = 0;
                this->index = 0;
                continue;
            }
            this->last_pos = this->tmp_pos + 1;
            this->_doing_speaker = good();
            if (!this->_doing_speaker) {
                this->last_pos = 0;
                this->index = -1;
            }
            this->index++;
        } else {
            this->tmp_pos = this->_sentence->what_said.find_first_of(' ', this->last_pos);
            if (this->tmp_pos > this->last_pos && this->tmp_pos <= this->what_said_length) {
                this->word = this->_sentence->what_said.substr(this->last_pos, this->tmp_pos - this->last_pos);
            } else {
                this->last_pos = std::string::npos;
                continue;
            }
            this->last_pos = this->tmp_pos + 1;
            this->index++;
        }
    } while (this->last_pos != std::string::npos && this->word.length() == 0);
}

bool FixedSentenceIterator::good() {
    return this->last_pos < (this->_doing_speaker ? this->speaker_name_length : this->what_said_length);
}