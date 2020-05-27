#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"

#include <iostream>

using namespace ScriptAI;

ModelDeleteAction::ModelDeleteAction(AIProgram *prog): MenuAction(prog) {

}

const char* ModelDeleteAction::label() {
	return "model delete";
}

std::string ModelDeleteAction::description() {
	return "Delete neural network";
}

void ModelDeleteAction::run() {
	if (this->_prog->m_model->exists()) {
		this->_prog->m_model->delete_model();
		std::cout << "Success!" << std::endl;
	} else {
		std::cerr << "Model does not exist" << std::endl;
	}
}
