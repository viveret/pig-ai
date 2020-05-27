#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Sentences/SentenceImportCmd.hpp"
#include "../../Data/Commands/Sentences/SentenceSourceGetCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace ScriptAI;
using namespace Sql;

SentenceImportFileAction::SentenceImportFileAction(AIProgram *prog): SentenceImportAction(prog) {

}

const char* SentenceImportFileAction::label() {
	return "sentence import file";
}

std::string SentenceImportFileAction::description() {
	return "Import sentences from a file to the neural network";
}

void SentenceImportFileAction::run() {
	this->ask_thread_count();
	std::cout << "Import path: ";
	std::string import_path;
	getline(std::cin, import_path);
	std::ifstream f(import_path);

	if (f.good()) {
		this->set_source(import_path);
		
		f.seekg(f.beg, std::ios_base::seekdir::_S_end);
		tiny_dnn::progress_display prog(f.tellg());
		f.seekg(f.beg, std::ios_base::seekdir::_S_beg);
		std::string line;
		while (std::getline(f, line).good()) {
			prog += line.length();
			this->items_to_parse.push(line);
		}

		this->wait_for_threads(&prog);
		std::cout << "Success!" << std::endl;
	} else {
		std::cout << "Could not open path" << std::endl;
	}
}
