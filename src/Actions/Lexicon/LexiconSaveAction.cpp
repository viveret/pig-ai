#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Lexicon/LexiconSaveCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

LexiconSaveAction::LexiconSaveAction(AIProgram *prog): MenuAction(prog) {

}

const char* LexiconSaveAction::label() {
	return "lexicon save";
}

std::string LexiconSaveAction::description() {
	return "Save lexicon for neural network";
}

void LexiconSaveAction::run() {
	std::cout << "Saving lexicon of size " << this->_prog->lexicon->count() << "..." << std::endl;
	tiny_dnn::progress_display prog(this->_prog->lexicon->count());
	LexiconSaveCmd cmd(SqlContext());
	for (size_t i = 0; i < this->_prog->lexicon->count(); i++, ++prog) {
		cmd.execute(this->_prog->lexicon->lexicon[i]);
	}
	std::cout << "Success!" << std::endl;
}
