#ifndef SQL_ConfigCountCmd_H
#define SQL_ConfigCountCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"

namespace PigAI {
    namespace Sql {
        class ConfigCountCmd: public SqlCommand<void*, int, int> {
            public:
            ConfigCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM ConfigState", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            int eachRow() override {
                return sqlite3_column_int(this->compiled, 0);
            }

            int allRows(const std::vector<int>& values) override { return values.at(0); }
        };
    }
}

#endif