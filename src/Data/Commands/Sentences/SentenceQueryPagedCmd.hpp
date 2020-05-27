#ifndef SQL_SentenceQueryPagedCmd_H
#define SQL_SentenceQueryPagedCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../Model/SentenceModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class SentenceCountCmd: public SqlCommand<void*, size_t, size_t> {
            public:
            SentenceCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM Sentences", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            size_t eachRow() override { return sqlite3_column_int(this->compiled, 0); }

            size_t allRows(const std::vector<size_t>& values) override { return values.at(0); }
        };

        class SentenceQueryPagedCmd: public SqlCommand<paged_position, SentenceModel, std::vector<SentenceModel>> {
            private:
            int _start, _count;

            public:
            SentenceQueryPagedCmd(sqlite3* sqlContext): SqlCommand("SELECT id, source_id, speaker_name, what_said FROM Sentences ORDER BY id ASC LIMIT @start,@count", sqlContext) {
                _start = this->paramIndex("@start");
                _count = this->paramIndex("@count");
            }

            int bind(paged_position value) override {
                auto err = sqlite3_bind_int(this->compiled, _start, value.start);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_int(this->compiled, _count, value.count);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            SentenceModel eachRow() override {
                return SentenceModel {  (size_t)sqlite3_column_int(this->compiled, 0),
                                        (size_t)sqlite3_column_int(this->compiled, 1),
                                        std::string((const char*)sqlite3_column_text(this->compiled, 2)),
                                        std::string((const char*)sqlite3_column_text(this->compiled, 3)) };
            }

            std::vector<SentenceModel> allRows(const std::vector<SentenceModel>& values) override { return values; }
        };
    }
}

#endif