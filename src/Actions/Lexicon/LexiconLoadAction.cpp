#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Lexicon/LexiconQueryPagedCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

LexiconLoadAction::LexiconLoadAction(AIProgram *prog): MenuAction(prog) {

}

const char* LexiconLoadAction::label() {
	return "lexicon load";
}

std::string LexiconLoadAction::description() {
	return "Load lexicon for neural network";
}

void LexiconLoadAction::run() {
	if (this->_prog->lexicon->is_empty() == false) {
		std::cout << "Lexicon not empty. Clear?";
		if (this->confirm()) {
			this->_prog->lexicon->clear();
		} else {
			return;
		}
	}

	auto count = LexiconCountCmd(SqlContext()).execute(nullptr);
	if (count > 0) {
		tiny_dnn::progress_display prog(count);
		LexiconQueryPagedCmd lexiconQuery(SqlContext());

		for (size_t id = 0, i = 0; id < count; id += 100) {
			auto items = lexiconQuery.execute(paged_position { i, 100 });
			for (auto it = items.begin(); it != items.end(); it++, i++, ++prog) {
				it->feature_index = i;
				this->_prog->lexicon->add(&*it);
			}
		}
		
		std::cout << "Success!" << std::endl;
	} else {
		std::cout << "Lexicon is empty." << std::endl;
	}
}
