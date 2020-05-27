#include "Arts.hpp"

#include <string>
#include <iostream>
#include <cstring>

using namespace ScriptAI;

StaticArt::StaticArt(AIProgram *prog): AbstractArt(prog) {
}

const char* StaticArt::label() { return ""; }

int StaticArt::frame_count() { return 1000; }

void StaticArt::render(int frame) {
    
}