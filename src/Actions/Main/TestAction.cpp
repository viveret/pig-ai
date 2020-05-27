#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"

#include <iostream>

using namespace ScriptAI;

TestAction::TestAction(AIProgram *prog): MenuAction(prog) {

}

const char* TestAction::label() {
	return "test";
}

std::string TestAction::description() {
	return "Test neural network";
}

void TestAction::run() {
	std::cout << "Initial speaker: ";
	std::string speaker;
	getline(std::cin, speaker);
	std::cout << "Initial seed: ";
	std::string line;
	getline(std::cin, line);
	this->_prog->m_model->test(speaker, line);
	std::cout << "Success!" << std::endl;
}
