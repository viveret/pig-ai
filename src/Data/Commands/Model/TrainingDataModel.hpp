#ifndef SQL_MODEL_TRAININGDATA_H
#define SQL_MODEL_TRAININGDATA_H

#include <string>
#include <vector>

namespace ScriptAI {
    namespace Sql {
        struct TrainingDataModel {
            size_t feature_index;
            size_t sentence_offset;
            size_t sentence_id;

            inline void copy(TrainingDataModel* other) {
                this->feature_index = other->feature_index;
                this->sentence_offset = other->sentence_offset;
                this->sentence_id = other->sentence_id;
            }
        };
    }
}

#endif