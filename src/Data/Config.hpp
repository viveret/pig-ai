#ifndef CONFIG_STATE_H
#define CONFIG_STATE_H

#include <map>
#include <vector>

namespace ScriptAI {
    namespace Config {
        class ConfigState {
            private:
            // Persisted
            std::string data;

            public:
            // Static
            std::string handle;
            size_t id;

            std::string displayName;
            std::string description;

            // Generated
            int accessCount;

            // Get Helpers
            bool IsNil();
            std::string asString();
            const char* asCString();
            bool asBool();
            int asInt();
            double asFloat();

            // Set Helpers
            void setNil();
            void setString(std::string value);
            void setBool(bool value);
            void setInt(int value);
            void setFloat(double value);
        };

        ConfigState* Get_State(std::string node);
        ConfigState* Get_State(size_t id);

        bool Exists();
        
        void Init();
        void Cleanup();
    }
}

#endif