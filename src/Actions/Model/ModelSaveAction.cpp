#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"

#include <iostream>

using namespace PigAI;

ModelSaveAction::ModelSaveAction(AIProgram *prog): MenuAction(prog) {

}

const char* ModelSaveAction::label() {
	return "model save";
}

std::string ModelSaveAction::description() {
	return "Save neural network";
}

void ModelSaveAction::run() {
	if (this->_prog->m_model->exists()) {
		this->_prog->m_model->save_model();
		std::cout << "Success!" << std::endl;
	} else {
		std::cerr << "Model does not exist" << std::endl;
	}
}
