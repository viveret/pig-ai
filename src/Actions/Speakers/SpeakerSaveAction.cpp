#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/SpeakerModel.hpp"
#include "../../Data/Commands/Speakers/SpeakerAddCmd.hpp"
#include "../../Data/Commands/Speakers/SpeakerClearCmd.hpp"
#include "../../Data/Commands/Speakers/SpeakerQueryPagedCmd.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

SpeakerSaveAction::SpeakerSaveAction(AIProgram *prog): MenuAction(prog) {

}

const char* SpeakerSaveAction::label() {
	return "speaker save";
}

std::string SpeakerSaveAction::description() {
	return "Save speakers for neural network";
}

void SpeakerSaveAction::run() {
	if (SpeakerCountCmd(SqlContext()).execute(nullptr) > 0) {
		std::cout << "Speakers already exist. Clear?";
		if (this->confirm()) {
			SpeakerClearCmd(SqlContext()).execute(nullptr);
		} else {
			return;
		}
	}

	std::cout << "Saving speaker list of size " << this->_prog->speakers->speaker_ids.size() << "..." << std::endl;
	SpeakerAddCmd cmd(SqlContext());
	for (size_t i = 0; i < this->_prog->speakers->speaker_ids.size(); i++) {
		cmd.execute(this->_prog->speakers->speaker_ids[i]);
	}
	std::cout << "Success!" << std::endl;
}
