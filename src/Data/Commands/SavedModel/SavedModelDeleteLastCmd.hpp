#ifndef SQL_SavedModelDeleteLastCmd_H
#define SQL_SavedModelDeleteLastCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class SavedModelDeleteLastCmd: public UpdateSqlCommand<void*> {
            public:
            SavedModelDeleteLastCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM SavedModels WHERE id=(SELECT MAX(id) FROM SavedModels)", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }
        };
    }
}

#endif