#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"

#include <iostream>

using namespace ScriptAI;

SentenceLoadAction::SentenceLoadAction(AIProgram *prog): MenuAction(prog) {

}

const char* SentenceLoadAction::label() {
	return "sentence load";
}

std::string SentenceLoadAction::description() {
	return "Load sentence for neural network";
}

void SentenceLoadAction::run() {
	this->_prog->m_model->load_model();
	std::cout << "Success!" << std::endl;
}
