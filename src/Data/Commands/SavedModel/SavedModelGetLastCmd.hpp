#ifndef SQL_SavedModelGetLastCmdCmd_H
#define SQL_SavedModelGetLastCmdCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../Model/SentenceModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class SavedModelGetLastCmd: public SqlCommand<void*, std::string, std::string> {
            public:
            SavedModelGetLastCmd(sqlite3* sqlContext): SqlCommand("SELECT model_data FROM SavedModels ORDER BY id DESC LIMIT 1", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            std::string eachRow() override { return std::string((const char*)sqlite3_column_text(this->compiled, 0)); }

            std::string allRows(const std::vector<std::string>& values) override { return values.size() > 0 ? values[0] : ""; }
        };
    }
}

#endif