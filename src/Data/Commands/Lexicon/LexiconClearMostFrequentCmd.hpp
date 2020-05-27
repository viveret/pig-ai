#ifndef SQL_LexiconClearMostFrequentCmd_H
#define SQL_LexiconClearMostFrequentCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class LexiconClearMostFrequentCmd: public UpdateSqlCommand<int> {
            public:
            LexiconClearMostFrequentCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM Lexicon WHERE label IN (SELECT label FROM Lexicon ORDER BY freq DESC LIMIT @amount)", sqlContext) { }
            // UPDATE sqlite_sequence SET seq=(SELECT MAX(id) FROM Lexicon) WHERE name='Lexicon'

            int bind(int amount) override {
                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@amount"), amount);
                if (err) {
                    return err;
                }
                return SQLITE_OK;
            }
        };
    }
}

#endif