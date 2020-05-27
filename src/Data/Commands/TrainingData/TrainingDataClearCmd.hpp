#ifndef SQL_TrainingDataClearCmd_H
#define SQL_TrainingDataClearCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class TrainingDataClearCmd: public UpdateSqlCommand<void*> {
            public:
            TrainingDataClearCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM TrainingDataWords", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }
        };
    }
}

#endif