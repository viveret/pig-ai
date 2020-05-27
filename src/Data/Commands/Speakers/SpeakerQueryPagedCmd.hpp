#ifndef SQL_SpeakerQueryPagedCmd_H
#define SQL_SpeakerQueryPagedCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../Model/LexiconModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class SpeakerCountCmd: public SqlCommand<void*, size_t, size_t> {
            public:
            SpeakerCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM Speakers", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            size_t eachRow() override { return (size_t)sqlite3_column_int(this->compiled, 0); }

            size_t allRows(const std::vector<size_t>& values) override { return values.at(0); }
        };
        
        class SpeakerQueryPagedCmd: public SqlCommand<paged_position, int, std::vector<int>> {
            public:
            SpeakerQueryPagedCmd(sqlite3* sqlContext): SqlCommand("SELECT lexicon_id FROM Speakers WHERE lexicon_id BETWEEN @start AND @start+@count-1 ORDER BY lexicon_id", sqlContext) { }

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

            int eachRow() override {
                return sqlite3_column_int(this->compiled, 0);
            }

            std::vector<int> allRows(const std::vector<int>& values) override {
                return values;
            }
        };
    }
}

#endif