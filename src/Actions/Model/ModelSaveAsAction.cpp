#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"

#include <iostream>

using namespace ScriptAI;

ModelSaveAsAction::ModelSaveAsAction(AIProgram *prog): MenuAction(prog) {

}

const char* ModelSaveAsAction::label() {
	return "model save as";
}

std::string ModelSaveAsAction::description() {
	return "Save neural network to custom path";
}

void ModelSaveAsAction::run() {
	if (this->_prog->m_model->exists()) {
		std::string save_path;
		getline(std::cin, save_path);
		this->_prog->m_model->save_model(save_path);
		std::cout << "Success!" << std::endl;
	} else {
		std::cerr << "Model does not exist" << std::endl;
	}
}
