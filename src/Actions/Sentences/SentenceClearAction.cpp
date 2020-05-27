#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Sentences/SentenceClearCmd.hpp"

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

SentenceClearAction::SentenceClearAction(AIProgram *prog): MenuAction(prog) {

}

const char* SentenceClearAction::label() {
	return "sentence clear";
}

std::string SentenceClearAction::description() {
	return "Clear sentences from neural network";
}

void SentenceClearAction::run() {
	SentenceClearCmd(SqlContext()).execute(nullptr);
	std::cout << "Success!" << std::endl;
}
