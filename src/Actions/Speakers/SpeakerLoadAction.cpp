#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/SpeakerModel.hpp"
#include "../../Data/Commands/Speakers/SpeakerQueryPagedCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>

using namespace ScriptAI;
using namespace Sql;

SpeakerLoadAction::SpeakerLoadAction(AIProgram *prog): MenuAction(prog) {

}

const char* SpeakerLoadAction::label() {
	return "speaker load";
}

std::string SpeakerLoadAction::description() {
	return "Load speakers for neural network";
}

void SpeakerLoadAction::run() {
	if (this->_prog->speakers->is_empty() == false) {
		std::cout << "Speakers list not empty. Clear?";
		if (this->confirm()) {
			this->_prog->speakers->clear();
		} else {
			return;
		}
	}

	auto max_id = SpeakerCountCmd(SqlContext()).execute(nullptr);
	tiny_dnn::progress_display prog(max_id);
	SpeakerQueryPagedCmd cmd(SqlContext());

	for (size_t i = 0; i < max_id;) {
		auto items = cmd.execute(paged_position { i, 100 });
		for (auto it = items.begin(); it != items.end(); it++, i++, ++prog) {
			this->_prog->speakers->add_speaker(*it);
		}
	}
	
	std::cout << "Success!" << std::endl;
}
