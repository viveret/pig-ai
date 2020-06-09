#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace PigAI;

HelpAction::HelpAction(AIProgram *prog): MenuAction(prog) {

}

const char* HelpAction::label() {
	return "help";
}

std::string HelpAction::description() {
	return "Display useful information about commands";
}

void HelpAction::run() {
	std::cout << "Commands:" << std::endl;

	int max_len = 0;
	for (auto it = this->_prog->actions.begin(); it != this->_prog->actions.end(); it++) {
		max_len = std::max(max_len, (int) strlen((*it)->label()));
	}

	max_len += 1;
	for (auto it = this->_prog->actions.begin(); it != this->_prog->actions.end(); it++) {
		std::cout << std::left << std::setw(max_len) << (*it)->label() << " - " << (*it)->description() << std::endl;
	}
}
