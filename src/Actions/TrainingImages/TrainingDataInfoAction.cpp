#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/TableCountCmd.hpp"
#include "../../Data/Commands/TrainingImages/TrainingImagesCountCmd.hpp"

using namespace PigAI;
using namespace Sql;

TrainingImagesInfoAction::TrainingImagesInfoAction(AIProgram *prog): MenuAction(prog) {

}

const char* TrainingImagesInfoAction::label() {
	return "training info";
}

std::string TrainingImagesInfoAction::description() {
	return "Display information about the neural network's training data";
}

void TrainingImagesInfoAction::run() {
	if (TrainingImagesCountCmd(SqlContext()).execute(nullptr) == 0) {
		std::cout << "Training data is empty";
	} else {
		std::cout << "Training data is ready";
	}
}
