#ifndef SQL_COMMAND_H
#define SQL_COMMAND_H

#include <sqlite3.h>
#include <vector>
#include <cassert>
#include <string>

namespace PigAI {
    namespace Sql {
        template <class TIn, class TRow, class TOut>
        class SqlCommand {
            public:
            SqlCommand(std::string text, sqlite3* sqlContext): text(text), sqlContext(sqlContext) {
                compile();
            }
            ~SqlCommand() {
                sqlite3_finalize(this->compiled);
                sqlContext = NULL;
            }

            std::string text;
            sqlite3* ctx;
            sqlite3_stmt* compiled = NULL;
            sqlite3* sqlContext;
            bool has_executed = false;

            void compile() {
                auto err = sqlite3_prepare_v3(sqlContext, this->text.c_str(), -1, SQLITE_PREPARE_PERSISTENT, &this->compiled, NULL);
                this->abort_if_error(err, "Command failed to compile statement");
            }

            void abort_if_error(int err, const char* msg) {
                if (err) {
                    printf("%s (%s)\n", msg, sqlite3_errmsg(sqlContext));
                    printf("\t\t%s", sqlite3_errmsg(sqlContext));
                    sqlite3_finalize(this->compiled);
                    assert(false);
                }
            }

            TOut execute(TIn value) {
                if (has_executed) {
                    sqlite3_reset(this->compiled);
                }
                auto err = this->bind(value);
                this->has_executed = true;
                this->abort_if_error(err, "Command failed to bind statement");
                
                std::vector<TRow> rows;
                while ((err = sqlite3_step(this->compiled)) == SQLITE_ROW)
                {
                    rows.push_back(this->eachRow());
                }

                if (err == SQLITE_DONE) {
                    return this->allRows(rows);
                } else {
                    this->abort_if_error(err, "Command failed to read statement results");
                }
            }

            int paramIndex(const char* paramName) {
                return sqlite3_bind_parameter_index(this->compiled, paramName);
            }
            
            virtual int bind(TIn value) = 0;
            virtual TRow eachRow() = 0;
            virtual TOut allRows(const std::vector<TRow>&) = 0;
        };
        
        template <class TIn>
        class UpdateSqlCommand: public SqlCommand<TIn, void*, void*> {
            public:
            UpdateSqlCommand(std::string text, sqlite3* sqlContext): SqlCommand<TIn, void*, void*>(text, sqlContext) { }

            void* eachRow() override { return nullptr; }

            void* allRows(const std::vector<void*>&) override { return nullptr; }
        };
    }
}

#endif