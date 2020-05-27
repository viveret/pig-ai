#include "Context.hpp"

#include <sqlite3.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>

#include "../Commands/TableExistsCmd.hpp"
#include "../Commands/TableCountCmd.hpp"
#include "../Commands/StaticUpdateCmd.hpp"

#include <tiny_dnn/config.h>
#include <tiny_dnn/io/display.h>

using namespace ScriptAI;
using namespace Sql;

auto MIGRATIONS_PATH = "migrations";

int CountUpgrades();
void SqlContext_Init_CheckForUpgrades();

struct System {
    int version;
    bool upgrading;
};

System _System {
    CountUpgrades(),
    false
};

int CountUpgrades() {
    struct stat stats;
    stat(MIGRATIONS_PATH, &stats);

    if (S_ISDIR(stats.st_mode)) {
        auto finalUpgrade = 0;

        DIR *dir = opendir(MIGRATIONS_PATH);
        struct dirent *ent;
        while ((ent = readdir (dir)) != NULL) {
            std::string scriptPath = ent->d_name;
            std::replace(scriptPath.begin(), scriptPath.end(), '_', ' ');
            std::stringstream path(scriptPath);

            int id;
            path >> id;
            if (id > finalUpgrade) {
                finalUpgrade = id;
            }
        }
        closedir(dir);
        return finalUpgrade;
    } else {
        std::cerr << "Migrations folder " << MIGRATIONS_PATH << " does not exist" << std::endl;
        return SQLITE_ERROR;
    }
}

class GetSystemCmd: public SqlCommand<void*, System*, System*> {
    public:
    GetSystemCmd(sqlite3* sqlContext): SqlCommand("SELECT version, upgrading FROM System", sqlContext) { }

    int bind(void*) override {
        return SQLITE_OK;
    }

    System* eachRow() override {
        return new System {
            sqlite3_column_int(this->compiled, 0),
            sqlite3_column_int(this->compiled, 1) == 1
        };
    }

    System* allRows(const std::vector<System*>& values) override {
        return values.size() > 0 ? values.at(0) : NULL;
    }
};

class SaveSystemCmd: public UpdateSqlCommand<System*> {
    public:
    SaveSystemCmd(sqlite3* sqlContext): UpdateSqlCommand("UPDATE System SET version=@version, upgrading=@upgrading", sqlContext) { }
    
    int bind(System* value) override {
        auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@version"), value->version);
        if (err) {
            return err;
        }

        err = sqlite3_bind_int(this->compiled, this->paramIndex("@upgrading"), value->upgrading);
        if (err) {
            return err;
        }

        return SQLITE_OK;
    }
};

std::string GetMigrationScript(int migrationId, bool isUpgrading) {
    std::string scriptPath;
    DIR *dir = opendir(MIGRATIONS_PATH);
    struct dirent *ent;
    while ((ent = readdir (dir)) != NULL) {
        scriptPath = ent->d_name;
        std::replace(scriptPath.begin(), scriptPath.end(), '_', ' ');
        std::stringstream path(scriptPath);

        int id;
        path >> id;
        if (id == migrationId) {
            scriptPath = ent->d_name;
            break;
        } else {
            scriptPath = "";
        }
    }
    closedir(dir);

    if (scriptPath.length() > 0) {
        scriptPath = std::string(MIGRATIONS_PATH) + "/" + scriptPath;
        std::ifstream migrationFile(scriptPath);
        if (migrationFile.good()) {
            std::stringstream migrationScript;
            int upgradeMode = 0;
            for (std::string line; getline(migrationFile, line);)
            {
                if (line == "upgrade:") {
                    upgradeMode = 1;
                } else if (line == "downgrade:") {
                    upgradeMode = 2;
                } else if ((isUpgrading && upgradeMode == 1) || (!isUpgrading && upgradeMode == 2)) {
                    migrationScript << line << "\n";
                }
            }

            return migrationScript.str();
        } else {
            std::cerr << "Failed to open " << scriptPath << ": file does not exist" << std::endl;;
            assert(false);
        }
    } else {
        std::cerr << "Failed run migration " << migrationId << ": file does not exist" << std::endl;
        assert(false);
    }
}

sqlite3* sqlContext = 0;

sqlite3* SqlContext() {
    return sqlContext;
}

void SqlContext_Init() {
    auto initSqlError = sqlite3_open_v2("current.sqlite", &sqlContext, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
    if (initSqlError != SQLITE_OK) {
        std::cerr << "Failed to open current.sqlite: " << sqlite3_errmsg(sqlContext) << std::endl;
        assert(false);
    }

    SqlContext_Init_CheckForUpgrades();
}

int SqlContext_Init_Upgrade(System* system) {
    system->upgrading = true;
    if (TableExistsCmd(sqlContext).execute("System")) {
        SaveSystemCmd(sqlContext).execute(system);
    }

    tiny_dnn::timer t;
    for (; system->version < _System.version; system->version++) {
        std::cout << "Upgrading from " << system->version <<" to " << (system->version + 1) << "...\n";
        auto script = GetMigrationScript(system->version + 1, true);
        char* err = NULL;
        sqlite3_exec(sqlContext, script.c_str(), NULL, NULL, &err);
        if (err) {
            std::cerr << "Upgrade script: " << script << std::endl;
            std::cerr << "Upgrade failed: " << err << std::endl;
            assert(false);
        }
        SaveSystemCmd(sqlContext).execute(system);
    }

    system->upgrading = false;   
    SaveSystemCmd(sqlContext).execute(system);
        
    std::cout << "Upgrade complete, " << t.elapsed() << "s elapsed." << std::endl;
    return SQLITE_OK;
}

int SqlContext_Init_Downgrade(System* system) {
    std::cout << "Downgrading from " << system->version + 1 << " to " << system->version << "...\n";
    auto script = GetMigrationScript(system->version + 1, false);
    char* err = NULL;
    sqlite3_exec(sqlContext, script.c_str(), NULL, NULL, &err);
    if (err) {
        std::cerr << "Downgrade script: " << script << std::endl;
        std::cerr << "Downgrade failed: " << err << std::endl;
        assert(false);
    }
    system->upgrading = false;   
    SaveSystemCmd(sqlContext).execute(system);
    
    return SQLITE_OK;
}

void SqlContext_Init_CheckForUpgrades() {
    System* system = NULL;
    if (TableExistsCmd(sqlContext).execute("System")) {
        system = GetSystemCmd(sqlContext).execute(NULL);
    }

    if (system) {
        printf("Save version: %d, executing version: %d, upgrading: %s.\n", system->version, _System.version, system->upgrading ? "yes" : "no");

        if (system && system->upgrading && system->version > 0) {
            printf("Failed to upgrade, reverting and then retrying upgrade.\n");
            // Revert to original save
            SqlContext_Init_Downgrade(system);
            // retry upgrade
            SqlContext_Init_Upgrade(system);
        } else if (system->version < _System.version) {
            SqlContext_Init_Upgrade(system);
        } else {
            printf("Nothing to upgrade.\n");
        }
        delete system;
    } else {
        printf("This is a fresh run! Exciting!\n");
        System tmp {
            0,
            false
        };
        SqlContext_Init_Upgrade(&tmp);
    }
}

void SqlContext_Clean() {
    auto initSqlError = sqlite3_close_v2(sqlContext);
    if (initSqlError != SQLITE_OK) {
        printf("ConfigState failed to close current.sqlite: %s\n", sqlite3_errmsg(sqlContext));
    }
}