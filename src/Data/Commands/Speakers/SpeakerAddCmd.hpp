#ifndef SQL_SpeakerAddCmd_H
#define SQL_SpeakerAddCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/SpeakerModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class SpeakerAddCmd: public UpdateSqlCommand<int> {
            public:
            SpeakerAddCmd(sqlite3* sqlContext): UpdateSqlCommand(
                "INSERT INTO Speakers(lexicon_id) SELECT @id "
                "WHERE NOT EXISTS (SELECT 1 FROM Speakers WHERE lexicon_id=@id)"
                , sqlContext) { }

            int bind(int value) override {
                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@id"), value);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };
    }
}

#endif