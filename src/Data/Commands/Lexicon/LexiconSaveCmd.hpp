#ifndef SQL_LexiconSaveCmd_H
#define SQL_LexiconSaveCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/LexiconModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class LexiconSaveCmd: public UpdateSqlCommand<LexiconFeatureModel> {
            public:
            LexiconSaveCmd(sqlite3* sqlContext): UpdateSqlCommand(
                "INSERT INTO Lexicon(label,occurrences,freq) "
                "VALUES(@label,@occurrences,@freq) "
                "ON CONFLICT(label) DO UPDATE "
                "SET freq=@freq,occurrences=@occurrences "
                , sqlContext) { }

            LexiconFeatureModel value;

            int bind(LexiconFeatureModel value) override {
                this->value = value;

                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@occurrences"), this->value.occurrences);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_double(this->compiled, this->paramIndex("@freq"), this->value.freq);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_text(this->compiled, this->paramIndex("@label"), this->value.label.c_str(), -1, nullptr);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };
    }
}

#endif