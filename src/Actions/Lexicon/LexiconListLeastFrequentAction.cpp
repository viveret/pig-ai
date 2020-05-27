#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/TableCountCmd.hpp"
#include "../../Data/Commands/Lexicon/LexiconQueryPagedCmd.hpp"
#include "../../Data/Commands/Lexicon/LexiconQueryLeastFrequentPagedCmd.hpp"

using namespace ScriptAI;
using namespace Sql;

LexiconListLeastFrequentAction::LexiconListLeastFrequentAction(AIProgram *prog): PagedAction(prog, 10) {

}

const char* LexiconListLeastFrequentAction::label() {
	return "lexicon list least frequent";
}

std::string LexiconListLeastFrequentAction::description() {
	return "List the least frequently used parts of the neural network's lexicon";
}

const char* LexiconListLeastFrequentAction::empty_msg() {
	return "Lexicon is empty";
}

size_t LexiconListLeastFrequentAction::get_count() {
	return LexiconCountCmd(SqlContext()).execute(nullptr);
}

size_t LexiconListLeastFrequentAction::run_paged(size_t start) {
	auto items = LexiconQueryLeastFrequentPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t i = 0;
	for (auto it = items.begin(); it != items.end(); it++, i++) {
		std::cout << i << ") " << it->label << " " << it->freq << std::endl;
	}

	return i;
}
