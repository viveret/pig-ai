#ifndef SQL_SavedModelCountCmd_H
#define SQL_SavedModelCountCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class SavedModelCountCmd: public SqlCommand<void*, size_t, size_t> {
            public:
            SavedModelCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM SavedModels", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            size_t eachRow() override { return (size_t)sqlite3_column_int(this->compiled, 0); }

            size_t allRows(const std::vector<size_t>& values) override { return values.at(0); }
        };
    }
}

#endif