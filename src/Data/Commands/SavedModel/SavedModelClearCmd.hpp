#ifndef SQL_SavedModelClearCmd_H
#define SQL_SavedModelClearCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace PigAI {
    namespace Sql {
        class SavedModelClearCmd: public UpdateSqlCommand<void*> {
            public:
            SavedModelClearCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM SavedModels", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }
        };
    }
}

#endif