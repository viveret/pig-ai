#ifndef PIGAI_PROGRAM_H
#define PIGAI_PROGRAM_H

#include <string>
#include <vector>
#include <map>
#include <regex>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
typedef int SOCKET;
#endif

#include "Data/Services/ConfigService.hpp"

namespace PigAI {
    class AIModelAdapter;
    class MenuAction;

    class AIProgram {
        private:
        char* _screen_data;

        public:
        AIProgram();

        bool should_continue;
        std::vector<MenuAction*> actions;
        std::map<std::string, std::string> action_aliases;

        AIModelAdapter* m_model;

        inline int node_scale() { return 1; }

        inline int input_channels() { return 3; }
        inline int input_width() { return 32; }
        inline int hidden_states_1() { return 32; }
        inline int hidden_states_2() { return 64; }
        inline int hidden_states_3() { return 64; }
        inline const char* DEFAULT_PORT() { return "9876"; }

        const char* label();
        const char* description();
        int run();

        void loop();

        void daemonNN();
        void incomingRequestNN(SOCKET client);
        void loadData();

        void lexicon_load(std::string import_path);
        void sentence_import(std::string import_path);
    };
}


#endif