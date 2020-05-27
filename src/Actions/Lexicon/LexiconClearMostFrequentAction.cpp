#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Lexicon/LexiconClearMostFrequentCmd.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

LexiconClearMostFrequentAction::LexiconClearMostFrequentAction(AIProgram *prog): MenuAction(prog) {

}

const char* LexiconClearMostFrequentAction::label() {
	return "lexicon clear most frequent";
}

std::string LexiconClearMostFrequentAction::description() {
	return "Clear most frequent parts of lexicon from neural network";
}

void LexiconClearMostFrequentAction::run() {
	std::cout << "Amount to clear (" << this->_prog->lexicon_size() << "): ";
	int amount;
	std::cin >> amount;
	std::string line;
	getline(std::cin, line);
	LexiconClearMostFrequentCmd(SqlContext()).execute(amount);
	this->_prog->lexicon->clear();
	std::cout << "Success! Lexicon cleared." << std::endl;
}
