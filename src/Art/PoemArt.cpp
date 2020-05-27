#include "Arts.hpp"

#include <string>
#include <iostream>
#include <cstring>

using namespace ScriptAI;

const char* POEMS[] = { 
    "Artificial intelligence is defined as the branch of science and technology that is concerned with the study of software and hardware to provide machines the ability to learn insights from data and the environment, and the ability to adapt in changing situations with high precision, accuracy and speed.",
    "Future Artificial Intelligence (AI) is more about human emotions, compassion, values and elimination of human sufferings.",
    "Emotional and creative intelligence: the final frontier. These are the voyages of AI. Its mission: to explore strange new emotions and creative worlds. To seek out empathy and the artistic muse. To boldly go where no robot has gone before!",
    nullptr
};
const char* POEM_LABELS[] = { 
    "― Amit Ray, Compassionate Artificial Superintelligence AI 5.0 - AI with Blockchain, BMI, Drone, IOT, and Biometric Technologies",
    "― Amit Ray, Compassionate Artificial intelligence",
    "― Khang Kijarro Nguyen",
    nullptr
};
int poem_count = 0;

PoemArt::PoemArt(AIProgram *prog): AbstractArt(prog), length(0) {
    if (poem_count == 0) {
        auto poem = POEMS;
        while (*poem != nullptr) {
            poem_count++;
            poem++;
        }
    }
}

const char* PoemArt::label() { return POEM_LABELS[this->_which]; }

int PoemArt::frame_count() { return this->length; }

void PoemArt::render(int frame) {
    std::cout << std::string(POEMS[this->_which]).substr(0, frame) << std::endl;
}

void PoemArt::reseed() {
    AbstractArt::reseed();
    this->_which = this->_seed * poem_count;
    this->length = strlen(this->label());
}
