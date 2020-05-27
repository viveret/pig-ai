#ifndef SQL_SentenceSourceGetCmdCmd_H
#define SQL_SentenceSourceGetCmdCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../Model/SentenceModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class SentenceSourceGetCmd: public SqlCommand<std::string, SentenceSourceModel, SentenceSourceModel> {
            public:
            SentenceSourceGetCmd(sqlite3* sqlContext): SqlCommand("SELECT id FROM SentenceSources WHERE file_path=@path", sqlContext) { }

            std::string path;

            int bind(std::string value) override {
                this->path = value;
                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@path"), value.c_str(), -1, nullptr);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            SentenceSourceModel eachRow() override {
                return SentenceSourceModel { (size_t)sqlite3_column_int(this->compiled, 1), this->path };
            }

            SentenceSourceModel allRows(const std::vector<SentenceSourceModel>& values) override { return values.size() > 0 ? values[0] : SentenceSourceModel { 0, "" }; }
        };
    }
}

#endif