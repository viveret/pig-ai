#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesClearCmd.hpp"

#include <iostream>

using namespace PigAI;
using namespace Sql;

SourceImagesClearAction::SourceImagesClearAction(AIProgram *prog): MenuAction(prog) {

}

const char* SourceImagesClearAction::label() {
	return "source images clear";
}

std::string SourceImagesClearAction::description() {
	return "Clear source images from neural network";
}

void SourceImagesClearAction::run() {
	SourceImagesClearCmd(SqlContext()).execute(nullptr);
	std::cout << "Success!" << std::endl;
}
