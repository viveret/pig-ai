#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Sentences/SentenceQueryPagedCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>
#include <limits.h>
#include <algorithm>

using namespace ScriptAI;
using namespace Sql;

LexiconBuildAction::LexiconBuildAction(AIProgram *prog): MenuAction(prog) {

}

const char* LexiconBuildAction::label() {
	return "lexicon build";
}

std::string LexiconBuildAction::description() {
	return "Build lexicon from sentences for neural network";
}

void LexiconBuildAction::run() {
	if (this->_prog->lexicon->is_empty() == false) {
		std::cout << "Lexicon not empty. Clear?";
		if (this->confirm()) {
			LexiconClearAction(this->_prog).run();
		} else {
			return;
		}
	}
	
	auto sentence_count = SentenceCountCmd(SqlContext()).execute(nullptr);
	if (sentence_count == 0) {
		std::cerr << "No sentences imported" << std::endl;
		return;
	}
	
	std::cout << "Converting " << sentence_count << " sentences to words..." << std::endl;
	tiny_dnn::progress_display prog(sentence_count);
	SentenceQueryPagedCmd querySentences(SqlContext());

	for (size_t i = 0; i < sentence_count; i += 100) {
		auto items = querySentences.execute(paged_position { i, 100 });
		for (auto it = items.begin(); it != items.end(); it++, ++prog) {
			this->_prog->lexicon->add_sentence(&*it);
		}
	}

	prog.restart(this->_prog->lexicon->count());
	std::cout << std::endl << "Done getting words, refreshing lexicon of size " << this->_prog->lexicon->count() << "..." << std::endl;

	size_t min_occ = INT_MAX, max_occ = 0;
	for (size_t i = 0; i < this->_prog->lexicon->count(); i++, ++prog) {
		min_occ = std::min(min_occ, this->_prog->lexicon->lexicon[i].occurrences);
		max_occ = std::max(max_occ, this->_prog->lexicon->lexicon[i].occurrences);
	}
	float diff_occ = max_occ - min_occ;
	std::cout << "Min occ: " << min_occ << ", max occ: " << max_occ << ", diff: " << diff_occ << std::endl;

	for (size_t i = 0; i < this->_prog->lexicon->count(); i++) {
		this->_prog->lexicon->lexicon[i].feature_index = i;
		this->_prog->lexicon->lexicon[i].freq = (this->_prog->lexicon->lexicon[i].occurrences - min_occ) / diff_occ;
	}

	std::cout << "Save?";
	if (this->confirm()) {
		LexiconSaveAction(this->_prog).run();
	}
	
	std::cout << "Success!" << std::endl;
}
