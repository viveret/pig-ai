#ifndef SQL_TrainingDataAddWordCmd_H
#define SQL_TrainingDataAddWordCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/TrainingDataModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class TrainingDataAddWordCmd: public UpdateSqlCommand<TrainingDataModel> {
            public:
            TrainingDataAddWordCmd(sqlite3* sqlContext): UpdateSqlCommand(
                "INSERT OR IGNORE INTO TrainingDataWords(sentence_id, sentence_offset, feature_index) "
                "VALUES(@id, @sentence_offset, @feature_index) "
                , sqlContext) {
                    this->_id = this->paramIndex("@id");
                    this->_sentence_offset = this->paramIndex("@sentence_offset");
                    this->_feature_index = this->paramIndex("@feature_index");
                }

            int _id, _sentence_offset, _feature_index;

            int bind(TrainingDataModel value) override {
                auto err = sqlite3_bind_int(this->compiled, this->_id, value.sentence_id);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_int(this->compiled, _sentence_offset, value.sentence_offset);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_int(this->compiled, _feature_index, value.feature_index);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };
    }
}

#endif