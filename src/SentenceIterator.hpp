#ifndef SCRIPTAI_SENTENCE_ITERATOR_H
#define SCRIPTAI_SENTENCE_ITERATOR_H

#include <string>
#include <regex>
#include <vector>

namespace ScriptAI {
    namespace Sql {
        struct SentenceModel;
    }

    class SentenceIterator {
        private:

        public:
        SentenceIterator();

        size_t index;
        std::string word;

        virtual bool good() = 0;
        virtual void next() = 0;
    };

    class RawSentenceIterator: public SentenceIterator {
        private:
        std::string _line, part;
        std::regex_token_iterator<std::string::iterator> part_iterator;
        size_t part_index;
        std::vector<std::string> words;

        public:
        RawSentenceIterator(std::string line);
        ~RawSentenceIterator();

        virtual bool good() override;
        virtual void next() override;

        bool assignment_acceptable();
    };

    class FixedSentenceIterator: public SentenceIterator {
        private:
        bool _doing_speaker;
        size_t last_pos, tmp_pos, speaker_name_length, what_said_length;
        const Sql::SentenceModel *_sentence;

        public:
        FixedSentenceIterator(const Sql::SentenceModel *sentence);
        FixedSentenceIterator(const Sql::SentenceModel &sentence);

        virtual bool good() override;
        virtual void next() override;
    };
}


#endif