#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Model/SpeakerModel.hpp"

#include <iostream>
#include <fstream>

using namespace ScriptAI;

SpeakerImportFileAction::SpeakerImportFileAction(AIProgram *prog): MenuAction(prog) {

}

const char* SpeakerImportFileAction::label() {
	return "speaker import file";
}

std::string SpeakerImportFileAction::description() {
	return "Add to neural network's known speakers by a file";
}

void SpeakerImportFileAction::run() {
	std::cout << "Import path: ";
	std::string import_path;
	getline(std::cin, import_path);
	std::ifstream f(import_path);
	size_t id;

	if (f.good()) {
		std::string line;
		while (std::getline(f, line).good()) {
			if (this->_prog->lexicon->try_get_id(line, id)) {
				this->_prog->speakers->add_speaker(id);
			}
		}
		std::cout << "Success!" << std::endl;
	} else {
		std::cout << "Could not open path" << std::endl;
	}
}
