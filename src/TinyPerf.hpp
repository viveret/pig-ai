#ifndef SCRIPTAI_TINYPERF_H
#define SCRIPTAI_TINYPERF_H

#include <memory>
#include <map>
#include <atomic>

namespace ScriptAI {
    class TinyPerfTracker;
    class TinyPerfCounter {
        private:
        TinyPerfTracker* tracker;
        int *value;

        public:
        TinyPerfCounter(TinyPerfTracker* tracker, int *value);
        ~TinyPerfCounter();
    };

    struct TinyPerfLocation {
        const char* file;
        const char* line;
        const char* function;

        size_t hash = -1;
    };
    
    class TinyPerfTracker {
        private:
        int *counts, *count_next;
        long *elapsed_millis;
        size_t count_capacity;
        std::map<int, int*> labels;
        std::map<int, TinyPerfLocation> locations;

        TinyPerfCounter track(const TinyPerfLocation& location);

        public:
        TinyPerfTracker(size_t capacity);
        ~TinyPerfTracker();

        size_t hash(const char* file, const char* line, const char* function);
        TinyPerfCounter track(const char* file, const char* line, const char* function);

        void reset();
        void tick();
    };
}



#endif