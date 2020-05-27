#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

ModelInitAction::ModelInitAction(AIProgram *prog): MenuAction(prog) {

}

const char* ModelInitAction::label() {
	return "model init";
}

std::string ModelInitAction::description() {
	return "Initialize model for neural network using lexicon";
}

void ModelInitAction::run() {
	if (this->_prog->m_model->exists()) {
		std::cout << "Model already exists. Delete?";
		if (this->confirm()) {
			this->_prog->m_model->delete_model();
		} else {
			return;
		}
	}
    
	if (_prog->lexicon_size() > 0) {
        _prog->m_model->create_model();
		std::cout << "Success!" << std::endl;
	} else {
		std::cout << "Lexicon empty." << std::endl;
	}
}
