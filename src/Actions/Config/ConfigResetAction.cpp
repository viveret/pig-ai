#include "../Actions.hpp"
#include "../../Program.hpp"
//#include "../../Data/Commands/Config/ConfigResetCmd.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

ConfigResetAction::ConfigResetAction(AIProgram *prog): MenuAction(prog) {

}

const char* ConfigResetAction::label() {
	return "config reset";
}

std::string ConfigResetAction::description() {
	return "Reset configuration options to defaults";
}

void ConfigResetAction::run() {
	//ConfigResetCmd(SqlContext()).execute(nullptr);
	std::cout << "Success!" << std::endl;
}
