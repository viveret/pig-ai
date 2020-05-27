#ifndef SQL_LexiconQueryPagedCmd_H
#define SQL_LexiconQueryPagedCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/SentenceModel.hpp"

namespace ScriptAI {
    namespace Sql {
        class SentenceImportCmd: public UpdateSqlCommand<SentenceModel> {
            public:
            SentenceImportCmd(sqlite3* sqlContext): UpdateSqlCommand("INSERT INTO Sentences(source_id, speaker_name, what_said) VALUES(@source_id, @speaker_name, @what_said)", sqlContext) { }

            std::string speaker_name, what_said;

            int bind(SentenceModel value) override {
				// std::cout << '"' << value.speaker_name << "\" said \"" << value.what_said << '"' << std::endl;
                speaker_name = value.speaker_name;
                what_said = value.what_said;

                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@source_id"), value.source_id);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_text(this->compiled, this->paramIndex("@speaker_name"), speaker_name.c_str(), -1, nullptr);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_text(this->compiled, this->paramIndex("@what_said"), what_said.c_str(), -1, nullptr);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };

        class SentenceSourceImportCmd: public UpdateSqlCommand<SentenceSourceModel> {
            public:
            SentenceSourceImportCmd(sqlite3* sqlContext): UpdateSqlCommand("INSERT OR IGNORE INTO SentenceSources(file_path) VALUES(@file_path)", sqlContext) { }

            std::string path;

            int bind(SentenceSourceModel value) override {
                this->path = value.file_path;
                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@file_path"), this->path.c_str(), -1, nullptr);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };
    }
}

#endif