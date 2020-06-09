#ifndef SQL_SourceImagesClearCmd_H
#define SQL_SourceImagesClearCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace PigAI {
    namespace Sql {
        class SourceImagesClearCmd: public UpdateSqlCommand<void*> {
            public:
            SourceImagesClearCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM SourceImages; DELETE FROM TrainingImages; DELETE FROM sqlite_sequence WHERE name='SourceImages';", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }
        };
    }
}

#endif