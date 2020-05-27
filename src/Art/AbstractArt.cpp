#include "Arts.hpp"

#include <time.h>
#include <stdlib.h>

using namespace ScriptAI;

AbstractArt::AbstractArt(AIProgram *prog): _prog(prog) {
}

void AbstractArt::reseed() {
    srand(time(nullptr));
    this->_seed = (double)rand()/RAND_MAX;
}