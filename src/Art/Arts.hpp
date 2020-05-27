#ifndef SCRIPTAI_ARTS_H
#define SCRIPTAI_ARTS_H

#include "AbstractArt.hpp"

namespace ScriptAI {
    class SwirlArt: public AbstractArt {
        protected:
        int _palette;

        public:
        SwirlArt(AIProgram *prog);

        virtual const char* label() override;
        virtual int frame_count() override;
        virtual void render(int frame) override;
    };

    class StaticArt: public AbstractArt {
        protected:
        int _which;

        public:
        StaticArt(AIProgram *prog);

        virtual const char* label() override;
        virtual int frame_count() override;
        virtual void render(int frame) override;
    };

    class FixedSentenceIterator;
    class PoemArt: public AbstractArt {
        protected:
        int _which, length;

        public:
        PoemArt(AIProgram *prog);

        virtual const char* label() override;
        virtual int frame_count() override;
        virtual void render(int frame) override;
        virtual void reseed() override;
    };
}

#endif