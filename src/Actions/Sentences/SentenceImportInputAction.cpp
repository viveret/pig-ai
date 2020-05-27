#include "../Actions.hpp"

#include <iostream>

using namespace ScriptAI;

SentenceImportInputAction::SentenceImportInputAction(AIProgram *prog): SentenceImportAction(prog) {

}

const char* SentenceImportInputAction::label() {
	return "sentence import input";
}

std::string SentenceImportInputAction::description() {
	return "Import sentences from standard input to the neural network";
}

void SentenceImportInputAction::run() {
	this->ask_thread_count();
	this->set_source("cin");

	std::string line;
	while (true) {
		std::cout << "import (q to quit): ";
		getline(std::cin, line);
		if (line != "q") {
			this->items_to_parse.push(line);
		} else {
			break;
		}
	}

	this->wait_for_threads(nullptr);
	std::cout << "Success!" << std::endl;
}
