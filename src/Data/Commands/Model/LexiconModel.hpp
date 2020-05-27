#ifndef SQL_MODEL_LEXICON_H
#define SQL_MODEL_LEXICON_H

#include <string>
#include <vector>
#include <map>

namespace ScriptAI {
    namespace Sql {
        struct LexiconFeatureModel {
            size_t feature_index;
            size_t occurrences;
            float freq;
            std::string label;

            inline void copy(LexiconFeatureModel* other) {
                this->feature_index = other->feature_index;
                this->occurrences = other->occurrences;
                this->freq = other->freq;
                this->label = other->label;
            }
        };

        struct SentenceModel;

        class LexiconModel {
            private:
            std::map<size_t, size_t> _reverse;

            public:
            LexiconModel(int capacity);

            inline size_t count() { return _reverse.size(); }
            std::vector<LexiconFeatureModel> lexicon;

            bool is_empty();
            void clear();
            void add_sentence(SentenceModel *sentence);
            void add_word(std::string& word);
            void add(LexiconFeatureModel* word);
            void copy(LexiconFeatureModel* word);

            bool exists(std::string& word);
            bool try_get_id(std::string& word, size_t &id);
            std::string select_one(std::vector<double>& probs);

            size_t hash(std::string& word);
        };
    }
}

#endif