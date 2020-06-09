#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Regex.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesAddCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace PigAI;
using namespace Sql;

SourceImagesAddFileAction::SourceImagesAddFileAction(AIProgram *prog): SourceImagesAddAction(prog) {

}

const char* SourceImagesAddFileAction::label() {
	return "source images add file";
}

std::string SourceImagesAddFileAction::description() {
	return "Add source images from a file to the neural network";
}

void SourceImagesAddFileAction::run() {
	this->ask_thread_count();
	std::cout << "Add file: ";
	std::string import_path;
	getline(std::cin, import_path);
	std::ifstream f(import_path);

	if (!f.good()) {
		std::cerr << "Could not open path" << std::endl;
		return;
	}

	std::string category;
	getline(std::cin, category);

	int cat_id = 0;
	if (this->try_resolve_category(category, cat_id)) {
		this->items_to_load.push(PathAndCategory { import_path, cat_id });
		tiny_dnn::progress_display prog(0);
		this->wait_for_threads(&prog);
		std::cout << "Success!" << std::endl;
	} else {
		std::cerr << "Invalid category" << std::endl;
	}
}
