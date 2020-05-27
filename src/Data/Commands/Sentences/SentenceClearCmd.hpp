#ifndef SQL_SentenceClearCmd_H
#define SQL_SentenceClearCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class SentenceClearCmd: public UpdateSqlCommand<void*> {
            public:
            SentenceClearCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM Sentences; DELETE FROM SentenceSources; DELETE FROM sqlite_sequence WHERE name='Sentences';", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }
        };
    }
}

#endif