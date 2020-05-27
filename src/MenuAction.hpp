#ifndef SCRIPTAI_MENU_ACTION_H
#define SCRIPTAI_MENU_ACTION_H

#include <string>

namespace ScriptAI {
    class AIProgram;
    class MenuAction {
        protected:
        AIProgram *_prog;

        public:
        MenuAction(AIProgram *prog);
        ~MenuAction();

        bool confirm();

        virtual const char* label() = 0;
        virtual std::string description() = 0;
        virtual void run() = 0;
    };
}

#endif