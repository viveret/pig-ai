#ifndef SQL_ConfigQueryPagedCmd_H
#define SQL_ConfigQueryPagedCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../../Config.hpp"

namespace ScriptAI {
    namespace Sql {
        class ConfigQueryPagedCmd: public SqlCommand<paged_position, Config::ConfigState, std::vector<Config::ConfigState>> {
            public:
            ConfigQueryPagedCmd(sqlite3* sqlContext): SqlCommand("SELECT id, handle, value FROM ConfigState WHERE id BETWEEN @start AND @start+@count-1 ORDER BY id ASC", sqlContext) { }

            int bind(paged_position value) override {
                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@start"), value.start);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_int(this->compiled, this->paramIndex("@count"), value.count);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            Config::ConfigState eachRow() override {
                Config::ConfigState ret;
                ret.id = sqlite3_column_int(this->compiled, 0);
                ret.handle = (const char*)sqlite3_column_text(this->compiled, 1);
                ret.setString((const char*)sqlite3_column_text(this->compiled, 2));
                return ret;
            }

            std::vector<Config::ConfigState> allRows(const std::vector<Config::ConfigState>& values) override { return values; }
        };
    }
}

#endif