#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"

#include <iostream>

using namespace ScriptAI;

ModelResetWeightsAction::ModelResetWeightsAction(AIProgram *prog): MenuAction(prog) {

}

const char* ModelResetWeightsAction::label() {
	return "model reset weights";
}

std::string ModelResetWeightsAction::description() {
	return "Reset neural network's weights";
}

void ModelResetWeightsAction::run() {
	if (this->_prog->m_model->exists()) {
		this->_prog->m_model->reset();
		std::cout << "Success!" << std::endl;
	} else {
		std::cerr << "Model does not exist" << std::endl;
	}
}
