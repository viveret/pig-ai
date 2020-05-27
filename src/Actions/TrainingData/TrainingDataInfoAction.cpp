#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/TableCountCmd.hpp"
#include "../../Data/Commands/TrainingData/TrainingDataSentenceCountCmd.hpp"

using namespace ScriptAI;
using namespace Sql;

TrainingDataInfoAction::TrainingDataInfoAction(AIProgram *prog): MenuAction(prog) {

}

const char* TrainingDataInfoAction::label() {
	return "training info";
}

std::string TrainingDataInfoAction::description() {
	return "Display information about the neural network's training data";
}

void TrainingDataInfoAction::run() {
	if (TrainingDataSentenceCountCmd(SqlContext()).execute(nullptr) == 0) {
		std::cout << "Training data is empty";
	} else {
		std::cout << "Training data is ready";
	}
}
