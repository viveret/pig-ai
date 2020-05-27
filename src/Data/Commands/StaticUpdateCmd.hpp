#ifndef SQL_StaticUpdateCmd_H
#define SQL_StaticUpdateCmd_H

#include "../Sql/Command.hpp"
#include "../Sql/Context.hpp"

namespace ScriptAI {
    namespace Sql {
        class StaticUpdateCmd: public SqlCommand<void*, void*, void*> {
            public:
            StaticUpdateCmd(std::string text, sqlite3* sqlContext): SqlCommand(text, sqlContext) { }

            void execute() {
                SqlCommand::execute(NULL);
            }

            int bind(void*) override {
                return SQLITE_OK;
            }

            void* eachRow() override {
                return NULL;
            }

            void* allRows(const std::vector<void*>&) override {
                return NULL;
            }
        };
    }
}

#endif