
#ifndef SQL_MODEL_SPEAKER_H
#define SQL_MODEL_SPEAKER_H

#include <string>
#include <vector>

namespace ScriptAI {
    namespace Sql {
        class LexiconModel;
        class SpeakerModel {
            public:
            SpeakerModel(size_t capacity, LexiconModel *lexicon);

            std::vector<size_t> speaker_ids;
            LexiconModel *lexicon;

            bool is_empty();
            void clear();
            void add_speaker(size_t id);
            void add_speaker(std::string name);
            void copy(SpeakerModel *other);
        };
    }
}

#endif