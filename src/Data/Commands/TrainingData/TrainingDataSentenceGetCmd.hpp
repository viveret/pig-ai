#ifndef SQL_TrainingDataSentenceGetCmd_H
#define SQL_TrainingDataSentenceGetCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/TrainingDataModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class TrainingDataSentenceGetCmd: public SqlCommand<size_t, TrainingDataModel, std::vector<TrainingDataModel>> {
            public:
            TrainingDataSentenceGetCmd(sqlite3* sqlContext): SqlCommand("SELECT feature_index, sentence_offset FROM TrainingDataWords WHERE sentence_id=@sentence_id ORDER BY sentence_offset ASC", sqlContext) { }

            size_t sentence_id;
            
            int bind(size_t sentence_id) override {
                this->sentence_id = sentence_id;
                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@sentence_id"), sentence_id);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            TrainingDataModel eachRow() override {
                return TrainingDataModel {
                    (size_t)sqlite3_column_int(this->compiled, 0),
                    (size_t)sqlite3_column_int(this->compiled, 1),
                    sentence_id
                };
            }

            std::vector<TrainingDataModel> allRows(const std::vector<TrainingDataModel>& values) override { return values; }
        };
    }
}

#endif