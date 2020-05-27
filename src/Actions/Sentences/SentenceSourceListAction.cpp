#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Sentences/SentenceSourceQueryPagedCmd.hpp"

#include <math.h>

using namespace ScriptAI;
using namespace Sql;

SentenceSourceListAction::SentenceSourceListAction(AIProgram *prog): PagedAction(prog, 10) {

}

const char* SentenceSourceListAction::label() {
	return "sentence source list";
}

std::string SentenceSourceListAction::description() {
	return "List the neural network's known sentence sources";
}

const char* SentenceSourceListAction::empty_msg() {
	return "Sentence source list is empty";
}

size_t SentenceSourceListAction::get_count() {
	return SentenceSourceCountCmd(SqlContext()).execute(nullptr);
}

size_t SentenceSourceListAction::run_paged(size_t start) {
	auto items = SentenceSourceQueryPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t max_id = 0;
	for (auto it = items.begin(); it != items.end(); it++, max_id = std::max(max_id, it->id)) {
		std::cout << (*it).id << ") " << (*it).file_path << std::endl;
	}

	return max_id;
}
