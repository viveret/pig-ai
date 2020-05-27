#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/TableCountCmd.hpp"
#include "../../Data/Commands/Lexicon/LexiconQueryPagedCmd.hpp"
#include "../../Data/Commands/Lexicon/LexiconQueryMostFrequentPagedCmd.hpp"

using namespace ScriptAI;
using namespace Sql;

LexiconListMostFrequentAction::LexiconListMostFrequentAction(AIProgram *prog): PagedAction(prog, 10) {

}

const char* LexiconListMostFrequentAction::label() {
	return "lexicon list most frequent";
}

std::string LexiconListMostFrequentAction::description() {
	return "List the neural network's lexicon";
}

const char* LexiconListMostFrequentAction::empty_msg() {
	return "Lexicon is empty";
}

size_t LexiconListMostFrequentAction::get_count() {
	return LexiconCountCmd(SqlContext()).execute(nullptr);
}

size_t LexiconListMostFrequentAction::run_paged(size_t start) {
	auto items = LexiconQueryMostFrequentPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t i = 0;
	for (auto it = items.begin(); it != items.end(); it++, i++) {
		std::cout << i << ") " << it->label << " " << it->freq << std::endl;
	}

	return i;
}
