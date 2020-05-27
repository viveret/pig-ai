#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Lexicon/LexiconClearCmd.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

LexiconClearAction::LexiconClearAction(AIProgram *prog): MenuAction(prog) {

}

const char* LexiconClearAction::label() {
	return "lexicon clear";
}

std::string LexiconClearAction::description() {
	return "Clear lexicon from neural network";
}

void LexiconClearAction::run() {
	LexiconClearCmd(SqlContext()).execute(nullptr);
	this->_prog->lexicon->clear();
	std::cout << "Success!" << std::endl;
}
