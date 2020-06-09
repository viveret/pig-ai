#include "../Actions.hpp"
#include "../../Program.hpp"

#include <iostream>

using namespace PigAI;

ConfigDeleteAction::ConfigDeleteAction(AIProgram *prog): MenuAction(prog) {

}

const char* ConfigDeleteAction::label() {
	return "config delete";
}

std::string ConfigDeleteAction::description() {
	return "Delete a configuration option";
}

void ConfigDeleteAction::run() {
	std::string handle, value;
	std::cout << "Name: ";
	std::cin >> handle;

	auto state = ConfigService::Get_State(handle);
	if (!state->IsNil()) {
		state->setNil();
		ConfigService::Save(state);
		std::cout << "Success!" << std::endl;
	} else {
		std::cerr << "Invalid option name" << std::endl;
	}
}
