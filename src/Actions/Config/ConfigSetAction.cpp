#include "../Actions.hpp"
#include "../../Program.hpp"

#include <iostream>

using namespace PigAI;

ConfigSetAction::ConfigSetAction(AIProgram *prog): MenuAction(prog) {

}

const char* ConfigSetAction::label() {
	return "config set";
}

std::string ConfigSetAction::description() {
	return "Set a configuration option";
}

void ConfigSetAction::run() {
	std::string handle, value;
	std::cout << "Name: ";
	std::cin >> handle;

	std::cout << "Value: ";
	std::cin >> value;

	auto state = ConfigService::Get_State(handle);
	if (!state->IsNil()) {
		state->setString(value);
		std::cout << "Success!" << std::endl;
	} else {
		std::cerr << "Invalid option name" << std::endl;
	}
}
