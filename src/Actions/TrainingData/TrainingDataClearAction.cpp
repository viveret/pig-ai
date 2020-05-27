#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/TrainingData/TrainingDataClearCmd.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

TrainingDataClearAction::TrainingDataClearAction(AIProgram *prog): MenuAction(prog) {

}

const char* TrainingDataClearAction::label() {
	return "training clear";
}

std::string TrainingDataClearAction::description() {
	return "Clear training data from neural network";
}

void TrainingDataClearAction::run() {
	TrainingDataClearCmd(SqlContext()).execute(nullptr);
	std::cout << "Success!" << std::endl;
}
