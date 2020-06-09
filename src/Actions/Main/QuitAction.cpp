#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"

#include <iostream>

using namespace PigAI;

QuitAction::QuitAction(AIProgram *prog): MenuAction(prog) {

}

const char* QuitAction::label() {
	return "quit";
}

std::string QuitAction::description() {
	return "Quit program. Can also use 'q' or 'exit'";
}

void QuitAction::run() {
	_prog->should_continue = false;
}
