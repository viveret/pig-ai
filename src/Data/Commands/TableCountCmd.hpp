#ifndef SQL_TableCountCmd_H
#define SQL_TableCountCmd_H

#include "../Sql/Command.hpp"
#include "../Sql/Context.hpp"

namespace PigAI {
    namespace Sql {
        class TableCountCmd: public SqlCommand<std::string, int, int> {
            public:
            TableCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM @name", sqlContext) { }

            int bind(std::string value) override {
                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@name"), value.c_str(), -1, NULL);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            int eachRow() override {
                return sqlite3_column_int(this->compiled, 1);
            }

            int allRows(const std::vector<int>& values) override {
                return values.at(0);
            }
        };
    }
}

#endif