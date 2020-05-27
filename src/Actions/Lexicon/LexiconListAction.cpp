#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/TableCountCmd.hpp"
#include "../../Data/Commands/Lexicon/LexiconQueryPagedCmd.hpp"

using namespace ScriptAI;
using namespace Sql;

LexiconListAction::LexiconListAction(AIProgram *prog): PagedAction(prog, 10) {

}

const char* LexiconListAction::label() {
	return "lexicon list";
}

std::string LexiconListAction::description() {
	return "List the neural network's lexicon";
}

const char* LexiconListAction::empty_msg() {
	return "Lexicon is empty";
}

size_t LexiconListAction::get_count() {
	return LexiconCountCmd(SqlContext()).execute(nullptr);
}

size_t LexiconListAction::run_paged(size_t start) {
	auto items = LexiconQueryPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t i = 0;
	for (auto it = items.begin(); it != items.end(); it++, i++) {
		std::cout << i << ") " << it->label << " " << it->freq << std::endl;
	}

	return i;
}
