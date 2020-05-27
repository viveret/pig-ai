#ifndef SCRIPTAI_PROGRAM_H
#define SCRIPTAI_PROGRAM_H

#include <string>
#include <vector>
#include <map>
#include <regex>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
typedef int SOCKET;
#endif

#include "TinyPerf.hpp"
#include "Data/Services/ConfigService.hpp"

namespace ScriptAI {
    class AIModelAdapter;
    class MenuAction;
    class AbstractArt;
    namespace Sql {
        class LexiconModel;
        class SpeakerModel;
    }

    class AIProgram {
        private:
        char* _screen_data;

        public:
        AIProgram();

        bool should_continue;
        std::vector<MenuAction*> actions;
        std::vector<AbstractArt*> art;
        std::map<std::string, std::string> action_aliases;
        TinyPerfTracker perf_tracker;

        AIModelAdapter* m_model;
        Sql::LexiconModel* lexicon;
        Sql::SpeakerModel* speakers;

        inline int speaker_capacity() { return 1500; }
        size_t lexicon_size();
        inline int memory_sequence_length() { return 12; }
        inline int hidden_states_1() { return 2000; }
        inline int hidden_states_2() { return 250; }
        inline int hidden_states_3() { return 50; }
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