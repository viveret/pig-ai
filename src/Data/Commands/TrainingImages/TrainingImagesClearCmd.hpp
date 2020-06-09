#ifndef SQL_TrainingImagesClearCmd_H
#define SQL_TrainingImagesClearCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace PigAI {
    namespace Sql {
        class TrainingImagesClearCmd: public UpdateSqlCommand<void*> {
            public:
            TrainingImagesClearCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM TrainingImages", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }
        };
    }
}

#endif