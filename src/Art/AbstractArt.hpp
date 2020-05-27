#ifndef SCRIPTAI_ABSTRACT_ART_H
#define SCRIPTAI_ABSTRACT_ART_H

namespace ScriptAI {
    class AIProgram;
    class AbstractArt {
        protected:
        AIProgram *_prog;
        double _seed;

        public:
        AbstractArt(AIProgram *prog);

        virtual void reseed();

        virtual const char* label() = 0;
        virtual int frame_count() = 0;
        virtual void render(int frame) = 0;
    };
}

#endif