#ifndef SQL_TrainingDataSentenceCountCmd_H
#define SQL_TrainingDataSentenceCountCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../Model/TrainingDataModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class TrainingDataSentenceCountCmd: public SqlCommand<void*, size_t, size_t> {
            public:
            TrainingDataSentenceCountCmd(sqlite3* sqlContext): SqlCommand("SELECT MAX(sentence_id) FROM TrainingDataWords", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            size_t eachRow() override { return (size_t)sqlite3_column_int(this->compiled, 0); }

            size_t allRows(const std::vector<size_t>& values) override { return values.at(0); }
        };
    }
}

#endif