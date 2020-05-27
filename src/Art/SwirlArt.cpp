#include "Arts.hpp"

#include <string>
#include <iostream>
#include <cstring>

using namespace ScriptAI;

SwirlArt::SwirlArt(AIProgram *prog): AbstractArt(prog) {
}

const char* SwirlArt::label() { return ""; }

int SwirlArt::frame_count() { return 1000; }

void SwirlArt::render(int frame) {
    
}