#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"

#include <iostream>

using namespace ScriptAI;

TrainAction::TrainAction(AIProgram *prog): MenuAction(prog) {

}

const char* TrainAction::label() {
	return "train";
}

std::string TrainAction::description() {
	return "Training neural network";
}

void TrainAction::run() {
	this->_prog->m_model->train();
	std::cout << "Success!" << std::endl;
}
