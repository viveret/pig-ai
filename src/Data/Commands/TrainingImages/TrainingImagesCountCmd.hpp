#ifndef SQL_TrainingImagesCountCmd_H
#define SQL_TrainingImagesCountCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"

namespace PigAI {
    namespace Sql {
        class TrainingImagesCountCmd: public SqlCommand<void*, size_t, size_t> {
            public:
            TrainingImagesCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM TrainingImages", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            size_t eachRow() override { return (size_t)sqlite3_column_int(this->compiled, 0); }

            size_t allRows(const std::vector<size_t>& values) override { return values.at(0); }
        };
    }
}

#endif