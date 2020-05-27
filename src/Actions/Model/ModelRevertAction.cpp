#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"

#include <iostream>

using namespace ScriptAI;

ModelRevertAction::ModelRevertAction(AIProgram *prog): MenuAction(prog) {

}

const char* ModelRevertAction::label() {
	return "model revert";
}

std::string ModelRevertAction::description() {
	return "Revert neural network to last save";
}

void ModelRevertAction::run() {
	if (this->_prog->m_model->exists()) {
		this->_prog->m_model->revert();
		std::cout << "Success!" << std::endl;
	} else {
		std::cerr << "Model does not exist" << std::endl;
	}
}
