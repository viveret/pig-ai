#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Sentences/SentenceQueryPagedCmd.hpp"

#include <math.h>

using namespace ScriptAI;
using namespace Sql;

SentenceListAction::SentenceListAction(AIProgram *prog): PagedAction(prog, 10) {

}

const char* SentenceListAction::label() {
	return "sentence list";
}

std::string SentenceListAction::description() {
	return "List the neural network's known sentences";
}

const char* SentenceListAction::empty_msg() {
	return "Sentence list is empty";
}

size_t SentenceListAction::get_count() {
	return SentenceCountCmd(SqlContext()).execute(nullptr);
}

size_t SentenceListAction::run_paged(size_t start) {
	auto items = SentenceQueryPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t i = 0;
	for (auto it = items.begin(); it != items.end(); it++, i++) {
		std::cout << (*it).id << "-" << (*it).source_id << ") ";
		std::cout << (*it).speaker_name << ": " << (*it).what_said << std::endl;
	}

	return i;
}
