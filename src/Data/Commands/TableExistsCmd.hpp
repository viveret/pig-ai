#ifndef SQL_TableExistsCmd_H
#define SQL_TableExistsCmd_H

#include "../Sql/Command.hpp"
#include "../Sql/Context.hpp"

namespace PigAI {
    namespace Sql {
        class TableExistsCmd: public SqlCommand<std::string, bool, bool> {
            public:
            TableExistsCmd(sqlite3* sqlContext): SqlCommand("SELECT 1 FROM sqlite_master WHERE type='table' AND name=@name;", sqlContext) { }

            int bind(std::string value) override {
                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@name"), value.c_str(), -1, NULL);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            bool eachRow() override {
                return true;
            }

            bool allRows(const std::vector<bool>& values) override {
                return values.size() > 0 && values.at(0);
            }
        };
    }
}

#endif