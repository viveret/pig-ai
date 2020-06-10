
#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Regex.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesAddCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <regex>

using namespace PigAI;
using namespace Sql;

ImagesImportFolderAction::ImagesImportFolderAction(AIProgram *prog): ImagesImportAction(prog) {

}

const char* ImagesImportFolderAction::label() {
	return "images import folder";
}

std::string ImagesImportFolderAction::description() {
	return "Add source images from a folder to the neural network";
}

void ImagesImportFolderAction::run() {
	this->ask_thread_count();
	std::cout << "Add folder: ";
	std::string import_path;
	getline(std::cin, import_path);

	std::cout << "Is folder organized by category";
	if (this->confirm()) {
		for (auto category = categories().begin(); category != categories().end(); category++) {
			int cat_id = 0;
			if (this->try_resolve_category(*category, cat_id)) {
				for (std::filesystem::recursive_directory_iterator i((import_path + "/" + *category).c_str()), end; i != end; ++i) {
					if (!is_directory(i->path())) {
						this->items_to_load.push(PathAndCategory { i->path().relative_path(), cat_id });
					}
				}
			} else {
				std::cerr << "Invalid category " << *category << std::endl;
			}
		}

		tiny_dnn::progress_display prog(0);
		this->wait_for_threads(&prog);
		std::cout << "Success!" << std::endl;
	} else {
		std::cout << "What is the category? (options are " << Config::Get_State("category")->asCString() << "): ";
		std::string category;
		getline(std::cin, category);

		int cat_id = 0;
		if (this->try_resolve_category(category, cat_id)) {
			for (std::filesystem::recursive_directory_iterator i(import_path.c_str()), end; i != end; ++i) {
				if (!is_directory(i->path())) {
					this->items_to_load.push(PathAndCategory { i->path().filename(), cat_id });
				}
			}
			tiny_dnn::progress_display prog(0);
			this->wait_for_threads(&prog);
			std::cout << "Success!" << std::endl;
		} else {
			std::cerr << "Invalid category" << std::endl;
		}
	}
}
