#ifndef SQL_LexiconClearLeastFrequentCmd_H
#define SQL_LexiconClearLeastFrequentCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class LexiconClearLeastFrequentCmd: public UpdateSqlCommand<int> {
            public:
            LexiconClearLeastFrequentCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE FROM Lexicon WHERE label IN (SELECT label FROM Lexicon ORDER BY freq ASC LIMIT @amount)", sqlContext) { }

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