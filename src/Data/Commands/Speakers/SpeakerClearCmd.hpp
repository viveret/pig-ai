#ifndef SQL_SpeakerClearCmd_H
#define SQL_SpeakerClearCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class SpeakerClearCmd: public UpdateSqlCommand<void*> {
            public:
            SpeakerClearCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM Speakers; DELETE FROM sqlite_sequence WHERE name='Speakers'", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }
        };
    }
}

#endif