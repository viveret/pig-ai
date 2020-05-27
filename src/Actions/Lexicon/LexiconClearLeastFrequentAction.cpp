#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Lexicon/LexiconClearLeastFrequentCmd.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

LexiconClearLeastFrequentAction::LexiconClearLeastFrequentAction(AIProgram *prog): MenuAction(prog) {

}

const char* LexiconClearLeastFrequentAction::label() {
	return "lexicon clear least frequent";
}

std::string LexiconClearLeastFrequentAction::description() {
	return "Clear least frequent parts of lexicon from neural network";
}

void LexiconClearLeastFrequentAction::run() {
	std::cout << "Amount to clear (" << this->_prog->lexicon_size() << "): ";
	int amount;
	std::cin >> amount;
	std::string line;
	getline(std::cin, line);
	LexiconClearLeastFrequentCmd(SqlContext()).execute(amount);
	this->_prog->lexicon->clear();
	std::cout << "Success! Lexicon cleared." << std::endl;
}
