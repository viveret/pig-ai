#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

ModelLoadAction::ModelLoadAction(AIProgram *prog): MenuAction(prog) {

}

const char* ModelLoadAction::label() {
	return "model load";
}

std::string ModelLoadAction::description() {
	return "Load model (weights) for neural network";
}

void ModelLoadAction::run() {
	if (this->_prog->m_model->exists()) {
		if (this->_prog->m_model->has_saved_model()) {
			_prog->m_model->load_model();
			std::cout << "Success!" << std::endl;
		} else {
			std::cerr << "No saved model to load from." << std::endl;
		}
	} else {
		std::cerr << "Model does not exist" << std::endl;
	}
}
