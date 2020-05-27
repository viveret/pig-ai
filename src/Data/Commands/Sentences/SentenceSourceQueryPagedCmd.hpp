#ifndef SQL_SentenceSourceQueryPagedCmd_H
#define SQL_SentenceSourceQueryPagedCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../Model/SentenceModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class SentenceSourceCountCmd: public SqlCommand<void*, size_t, size_t> {
            public:
            SentenceSourceCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM SentenceSources", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            size_t eachRow() override { return (size_t)sqlite3_column_int(this->compiled, 0); }

            size_t allRows(const std::vector<size_t>& values) override { return values.at(0); }
        };

        class SentenceSourceQueryPagedCmd: public SqlCommand<paged_position, SentenceSourceModel, std::vector<SentenceSourceModel>> {
            public:
            SentenceSourceQueryPagedCmd(sqlite3* sqlContext): SqlCommand("SELECT id, file_path FROM SentenceSources ORDER BY id ASC LIMIT @start,@count", sqlContext) { }

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

            SentenceSourceModel eachRow() override {
                return SentenceSourceModel { (size_t)sqlite3_column_int(this->compiled, 0),
                                        std::string((const char*)sqlite3_column_text(this->compiled, 1)) };
            }

            std::vector<SentenceSourceModel> allRows(const std::vector<SentenceSourceModel>& values) override { return values; }
        };
    }
}

#endif