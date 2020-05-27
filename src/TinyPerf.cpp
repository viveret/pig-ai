#include "TinyPerf.hpp"

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>

using namespace ScriptAI;

TinyPerfCounter::TinyPerfCounter(TinyPerfTracker* tracker, int *value): tracker(tracker), value(value) {
    (*this->value)++;
}

TinyPerfCounter::~TinyPerfCounter() {
    (*this->value)--;
}



TinyPerfTracker::TinyPerfTracker(size_t capacity): count_capacity(capacity) {
    this->counts = new int[capacity];
    this->count_next = counts;
}

TinyPerfTracker::~TinyPerfTracker() {
    this->labels.clear();
    delete this->counts;
}

TinyPerfCounter TinyPerfTracker::track(const TinyPerfLocation& location) {
    auto e = this->labels.find(location.hash);
    if (e != this->labels.end()) {
        return TinyPerfCounter(this, e->second);
    } else if (this->labels.size() < this->count_capacity) {
        auto next = count_next;
        this->labels[location.hash] = next;
        count_next++;
        return TinyPerfCounter(this, next);
    } else {
        std::cerr << "Performance tracker overflow" << std::endl;
        assert(false);
    }
}

TinyPerfCounter TinyPerfTracker::track(const char* file, const char* line, const char* function) {
    return this->track(TinyPerfLocation { file, line, function, this->hash(file, line, function) });
}

size_t TinyPerfTracker::hash(const char* file, const char* line, const char* function) {
    std::stringstream combined;
    combined << file << line << function;
    return std::hash<std::string>{}(combined.str());
}