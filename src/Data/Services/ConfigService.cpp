#include "ConfigService.hpp"

#include "../Sql/Command.hpp"
#include "../Sql/Context.hpp"

#include "../Commands/Config/ConfigQueryPagedCmd.hpp"

namespace PigAI {
    namespace ConfigService {
        class UpdateConfigArgs {
            public:
            UpdateConfigArgs(std::string handle, std::string value): handle(handle), value(value) {}

            std::string handle;
            std::string value;
        };

        class DeleteConfigCmd: public Sql::UpdateSqlCommand<Config::ConfigState*> {
            public:
            DeleteConfigCmd(sqlite3* sqlContext): UpdateSqlCommand("DELETE ConfigState WHERE handle=@handle", sqlContext) { }

            std::string handle;

            int bind(Config::ConfigState* value) override {
                handle = value->handle;
                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@handle"), handle.c_str(), -1, NULL);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };

        class UpdateConfigCmd: public Sql::SqlCommand<Config::ConfigState*, void*, void*> {
            public:
            UpdateConfigCmd(sqlite3* sqlContext): SqlCommand("UPDATE ConfigState SET value=@value WHERE handle=@handle", sqlContext) { }

            int bind(Config::ConfigState* value) override {
                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@handle"), value->handle.c_str(), -1, NULL);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_text(this->compiled, this->paramIndex("@value"), value->asCString(), -1, NULL);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            void* eachRow() override {
                return NULL;
            }

            void* allRows(const std::vector<void*>&) override {
                return NULL;
            }
        };

        class InsertConfigCmd: public Sql::SqlCommand<Config::ConfigState*, Config::ConfigState*, Config::ConfigState*> {
            public:
            InsertConfigCmd(sqlite3* sqlContext): SqlCommand("INSERT INTO ConfigState(handle, value) VALUES(@handle, @value)", sqlContext) { }

            Config::ConfigState* boundValue = NULL;
            int bind(Config::ConfigState* value) override {
                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@handle"), value->handle.c_str(), -1, NULL);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_text(this->compiled, this->paramIndex("@value"), value->asCString(), -1, NULL);
                if (err) {
                    return err;
                }

                this->boundValue = value;

                return SQLITE_OK;
            }

            Config::ConfigState* eachRow() override {
                this->boundValue->id = sqlite3_last_insert_rowid(this->ctx);
                return this->boundValue;
            }

            Config::ConfigState* allRows(const std::vector<Config::ConfigState*>& values) override {
                return values.size() > 0 ? values.at(0) : this->boundValue;
            }
        };

        template <class TIn>
        class SelectConfigCmd: public Sql::SqlCommand<TIn, Config::ConfigState*, std::vector<Config::ConfigState*>*> {
            public:
            SelectConfigCmd(std::string text, sqlite3* sqlContext): Sql::SqlCommand<TIn, Config::ConfigState*, std::vector<Config::ConfigState*>*>(text, sqlContext) { }

            Config::ConfigState* eachRow() override {
                auto ret = new Config::ConfigState();
                ret->id = (size_t)sqlite3_column_int(this->compiled, 0);
                ret->handle = std::string((const char*)sqlite3_column_text(this->compiled, 1));
                ret->setString((const char*)sqlite3_column_text(this->compiled, 2));
                return ret;
            }

            std::vector<Config::ConfigState*>* allRows(const std::vector<Config::ConfigState*>& value) override {
                return new std::vector<Config::ConfigState*>(value);
            }
        };

        class SelectConfigByHandleCmd: public SelectConfigCmd<const char*> {
            public:
            SelectConfigByHandleCmd(sqlite3* sqlContext): SelectConfigCmd("SELECT id, handle, value FROM ConfigState WHERE handle=@handle;", sqlContext) { }

            int bind(const char* value) override {
                auto err = sqlite3_bind_text(this->compiled, this->paramIndex("@handle"), value, -1, NULL);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };

        class SelectConfigByIdCmd: public SelectConfigCmd<size_t> {
            public:
            SelectConfigByIdCmd(sqlite3* sqlContext): SelectConfigCmd("SELECT id, handle, value FROM ConfigState WHERE id=@id", sqlContext) { }

            int bind(size_t value) override {
                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@id"), value);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };
    }
}
using namespace PigAI;
using namespace ConfigService;
using namespace Sql;

// Create and update
void PigAI::ConfigService::Save(Config::ConfigState* status) {
    UpdateConfigCmd cmd(SqlContext());
    cmd.execute(status);
}

// Read

Config::ConfigState* SelectFirst(std::vector<Config::ConfigState*>* results) {
    if (results->size() > 0) {
        auto first = *results->erase(results->begin());
        for (auto it = results->begin(); it != results->end(); it++) {
            delete *it;
        }
        delete results;
        return first;
    } else {
        return NULL;
    }
}

Config::ConfigState* PigAI::ConfigService::Get_State(std::string handle) {
    SelectConfigByHandleCmd cmd(SqlContext());
    auto ret = SelectFirst(cmd.execute(handle.c_str()));
        
    if (ret == NULL) {
        ret = new Config::ConfigState();
        ret->handle = handle;
        ret = InsertConfigCmd(SqlContext()).execute(ret);
    }

    return ret;
}

Config::ConfigState* PigAI::ConfigService::Get_State(size_t id) {
    SelectConfigByIdCmd cmd(SqlContext());
    return SelectFirst(cmd.execute(id));
}

std::vector<Config::ConfigState> PigAI::ConfigService::Get_Paged(size_t start, size_t count) {
    ConfigQueryPagedCmd cmd(SqlContext());
    return cmd.execute(paged_position { start, count });
}

// Delete
void PigAI::ConfigService::Delete(Config::ConfigState* status) {
    DeleteConfigCmd(SqlContext()).execute(status);
}