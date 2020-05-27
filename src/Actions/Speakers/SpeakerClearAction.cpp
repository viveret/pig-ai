#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/SpeakerModel.hpp"
#include "../../Data/Commands/Speakers/SpeakerClearCmd.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

SpeakerClearAction::SpeakerClearAction(AIProgram *prog): MenuAction(prog) {

}

const char* SpeakerClearAction::label() {
	return "speaker clear";
}

std::string SpeakerClearAction::description() {
	return "Clear speakers from neural network";
}

void SpeakerClearAction::run() {
	SpeakerClearCmd(SqlContext()).execute(nullptr);
	this->_prog->speakers->clear();
	std::cout << "Success!" << std::endl;
}
