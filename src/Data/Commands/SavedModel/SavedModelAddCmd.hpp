#ifndef SQL_SavedModelAddCmd_H
#define SQL_SavedModelAddCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace PigAI {
    namespace Sql {
        class SavedModelAddCmd: public UpdateSqlCommand<std::string> {
            public:
            SavedModelAddCmd(sqlite3* sqlContext): UpdateSqlCommand("INSERT INTO SavedModels(timestamp, model_data) VALUES(datetime('now'), @data)", sqlContext) { }

            std::string model_data;

            int bind(std::string value) override {
                model_data = value;

                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@data"), model_data.c_str(), -1, nullptr);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };
    }
}

#endif