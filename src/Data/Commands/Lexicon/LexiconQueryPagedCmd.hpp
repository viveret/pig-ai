#ifndef SQL_LexiconQueryPagedCmd_H
#define SQL_LexiconQueryPagedCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../Model/LexiconModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class LexiconCountCmd: public SqlCommand<void*, size_t, size_t> {
            public:
            LexiconCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM Lexicon", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            size_t eachRow() override { return (size_t)sqlite3_column_int(this->compiled, 0); }

            size_t allRows(const std::vector<size_t>& values) override { return values.at(0); }
        };
        
        class LexiconQueryPagedCmd: public SqlCommand<paged_position, LexiconFeatureModel, std::vector<LexiconFeatureModel>> {
            public:
            LexiconQueryPagedCmd(sqlite3* sqlContext): SqlCommand("SELECT occurrences, freq, label FROM Lexicon ORDER BY id LIMIT @start,@count", sqlContext) { }

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

            LexiconFeatureModel eachRow() override {
                return LexiconFeatureModel {
                    __UINT32_MAX__,
                    (size_t)sqlite3_column_int(this->compiled, 0),
                    (float)sqlite3_column_double(this->compiled, 1),
                    std::string((const char*)sqlite3_column_text(this->compiled, 2))
                };
            }

            std::vector<LexiconFeatureModel> allRows(const std::vector<LexiconFeatureModel>& values) override {
                return values;
            }
        };
    }
}

#endif