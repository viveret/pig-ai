#ifndef SQL_LexiconClearCmd_H
#define SQL_LexiconClearCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class LexiconClearCmd: public UpdateSqlCommand<void*> {
            public:
            LexiconClearCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM Lexicon; DELETE FROM sqlite_sequence WHERE name='Lexicon'", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }
        };
    }
}

#endif