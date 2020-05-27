#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Speakers/SpeakerQueryPagedCmd.hpp"

using namespace ScriptAI;
using namespace Sql;

SpeakerListAction::SpeakerListAction(AIProgram *prog): PagedAction(prog, 10) {

}

const char* SpeakerListAction::label() {
	return "speaker list";
}

std::string SpeakerListAction::description() {
	return "List the neural network's known speakers";
}

const char* SpeakerListAction::empty_msg() {
	return "Speakers list is empty";
}

size_t SpeakerListAction::get_count() {
	return SpeakerCountCmd(SqlContext()).execute(nullptr);
}

size_t SpeakerListAction::run_paged(size_t start) {
	auto items = SpeakerQueryPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t i = 0;
	for (auto it = items.begin(); it != items.end(); it++, i++) {
		std::cout << *it << ", ";
	}

	std::cout << std::endl;

	return i;
}
