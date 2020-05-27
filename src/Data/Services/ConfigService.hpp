#ifndef SCRIPTAI_CONFIGSERVICE_H
#define SCRIPTAI_CONFIGSERVICE_H

#include "../Config.hpp"

namespace ScriptAI
{
    namespace ConfigService
    {
        // Create and update
        void Save(Config::ConfigState* status);

        // Read
        Config::ConfigState* Get_State(std::string handle);
        Config::ConfigState* Get_State(size_t id);
        std::vector<Config::ConfigState> Get_Paged(size_t start, size_t count);

        // Delete
        void Delete(Config::ConfigState* status);
    } // namespace ConfigService
    
} // namespace ScriptAI



#endif