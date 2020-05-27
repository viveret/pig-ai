#ifndef SQL_MODEL_SENTENCE_H
#define SQL_MODEL_SENTENCE_H

#include <string>

namespace ScriptAI {
    namespace Sql {
        struct SentenceSourceModel {
            size_t id;
            std::string file_path;
        };

        struct SentenceModel {
            size_t id;
            size_t source_id;
            std::string speaker_name;
            std::string what_said;
        };
    }
}

#endif