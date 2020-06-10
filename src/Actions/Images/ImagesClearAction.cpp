#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesClearCmd.hpp"

#include <iostream>

using namespace PigAI;
using namespace Sql;

ImagesClearAction::ImagesClearAction(AIProgram *prog): MenuAction(prog) {

}

const char* ImagesClearAction::label() {
	return "images clear";
}

std::string ImagesClearAction::description() {
	return "Clear images from neural network";
}

void ImagesClearAction::run() {
	SourceImagesClearCmd(SqlContext()).execute(nullptr);
	std::cout << "Success!" << std::endl;
}
