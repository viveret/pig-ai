#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/Commands/SavedModel/SavedModelClearCmd.hpp"

#include <iostream>

using namespace PigAI;
using namespace Sql;

ModelDeleteSavedAction::ModelDeleteSavedAction(AIProgram *prog): MenuAction(prog) {

}

const char* ModelDeleteSavedAction::label() {
	return "model delete saved";
}

std::string ModelDeleteSavedAction::description() {
	return "Delete saved neural network models";
}

void ModelDeleteSavedAction::run() {
	SavedModelClearCmd(SqlContext()).execute(nullptr);
	std::cout << "Success!" << std::endl;
}
