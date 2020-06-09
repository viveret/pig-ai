#include "Config.hpp"
#include "Services/ConfigService.hpp"
#include "Sql/Context.hpp"

#include <tiny_dnn/config.h>
#include <tiny_dnn/io/display.h>

#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
#include <mutex>

#include <sqlite3.h>

namespace PigAI {
    namespace Config {
        std::map<std::string, ConfigState*> Cached_By_Handle;
        std::map<int, ConfigState*> Cached_By_Id;
        std::mutex Cache_Mutex;
    }
}
using namespace PigAI;
using namespace Config;

ConfigState* Config::Get_State(std::string node) {
    ConfigState* ret;
    Cache_Mutex.lock();

    auto find = Cached_By_Handle.find(node);
    if (find != Cached_By_Handle.end()) {
        ret = find->second;
    } else {
        ret = ConfigService::Get_State(node);
        ret->accessCount = 1;
        Cached_By_Id[ret->id] = Cached_By_Handle[ret->handle] = ret;
    }
    
    Cache_Mutex.unlock();

    return ret;
}

ConfigState* Config::Get_State(size_t id) {
    ConfigState* ret;
    Cache_Mutex.lock();

    auto find = Cached_By_Id.find(id);
    if (find != Cached_By_Id.end()) {
        ret = find->second;
    } else {
        ret = ConfigService::Get_State(id);
        if (ret) {
            ret->accessCount = 1;
            Cached_By_Id[ret->id] = Cached_By_Handle[ret->handle] = ret;
        }
    }
    
    Cache_Mutex.unlock();

    return ret;
}

bool Config::Exists() { return std::ifstream("current.sqlite").good(); }

void Config::Init() { SqlContext_Init(); }

void Config::Cleanup() {
    Cache_Mutex.lock();
    Cached_By_Handle.clear();
    for (auto it = Cached_By_Id.begin(); it != Cached_By_Id.end(); it++) {
        delete it->second;
    }
    Cached_By_Id.clear();
    Cache_Mutex.unlock();
    SqlContext_Clean();
}

// Get Helpers
bool ConfigState::IsNil() {
    return this->data.length() == 0;
}

std::string ConfigState::asString() {
    return this->data;
}

const char* ConfigState::asCString() {
    return this->data.c_str();
}

bool ConfigState::asBool() {
    return this->data == "1";
}

int ConfigState::asInt() {
    return atoi(this->data.c_str());
}

double ConfigState::asFloat() {
    return atof(this->data.c_str());
}

// Set Helpers
void ConfigState::setNil() {
    this->data = "";
    ConfigService::Save(this);
}
void ConfigState::setString(std::string value) {
    this->data = value;
    ConfigService::Save(this);
}

void ConfigState::setBool(bool value) {
    this->data = value ? "1" : "0";
    ConfigService::Save(this);
}

void ConfigState::setInt(int value) {
    std::stringstream ss;
    ss << value;
    this->data = ss.str();
    ConfigService::Save(this);
}

void ConfigState::setFloat(double value) {
    std::stringstream ss;
    ss << value;
    this->data = ss.str();
    ConfigService::Save(this);
}
