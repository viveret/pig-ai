#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Model/SpeakerModel.hpp"

#include <iostream>

using namespace ScriptAI;

SpeakerImportInputAction::SpeakerImportInputAction(AIProgram *prog): MenuAction(prog) {

}

const char* SpeakerImportInputAction::label() {
	return "speaker import input";
}

std::string SpeakerImportInputAction::description() {
	return "Add to neural network's known speakers through direct input";
}

void SpeakerImportInputAction::run() {
	std::string line;
	size_t id;
	while (true) {
		std::cout << "import (q to quit): ";
		getline(std::cin, line);
		if (line != "q") {
			if (this->_prog->lexicon->try_get_id(line, id)) {
				this->_prog->speakers->add_speaker(id);
			}
		} else {
			break;
		}
	}
	std::cout << "Success!" << std::endl;
}
