#include "Program.hpp"
#include "AIModelAdapter.hpp"
#include "MenuAction.hpp"

#include "Actions/Actions.hpp"
#include "Actions/TrainingData/TrainingDataActions.hpp"
#include "Art/Arts.hpp"
#include "Data/Commands/Model/LexiconModel.hpp"
#include "Data/Commands/Model/SpeakerModel.hpp"
#include "Data/Commands/Lexicon/LexiconQueryPagedCmd.hpp"

#include "StackOverflow.hpp"

#include <iostream>
#include <cctype>

using namespace ScriptAI;
using namespace Sql;

AIProgram::AIProgram(): should_continue(true), perf_tracker(1000), 
	m_model(nullptr), lexicon(nullptr), speakers(nullptr) {
		
	this->actions.push_back(new QuitAction(this));
	this->actions.push_back(new HelpAction(this));
	this->actions.push_back(new TestAction(this));
	this->actions.push_back(new TrainAction(this));
	this->actions.push_back(new DaemonAction(this));

	this->actions.push_back(new LexiconLoadAction(this));
	this->actions.push_back(new LexiconSaveAction(this));
	this->actions.push_back(new LexiconBuildAction(this));
	this->actions.push_back(new LexiconClearAction(this));
	this->actions.push_back(new LexiconClearLeastFrequentAction(this));
	this->actions.push_back(new LexiconClearMostFrequentAction(this));
	this->actions.push_back(new LexiconListAction(this));
	this->actions.push_back(new LexiconListMostFrequentAction(this));
	this->actions.push_back(new LexiconListLeastFrequentAction(this));

	this->actions.push_back(new ModelInitAction(this));
	this->actions.push_back(new ModelRevertAction(this));
	this->actions.push_back(new ModelSaveAction(this));
	this->actions.push_back(new ModelSaveAsAction(this));
	this->actions.push_back(new ModelLoadAction(this));
	this->actions.push_back(new ModelDeleteSavedAction(this));
	this->actions.push_back(new ModelDeleteAction(this));
	this->actions.push_back(new ModelResetWeightsAction(this));

	this->actions.push_back(new SpeakerLoadAction(this));
	this->actions.push_back(new SpeakerSaveAction(this));
	this->actions.push_back(new SpeakerImportInputAction(this));;
	this->actions.push_back(new SpeakerImportFileAction(this));
	this->actions.push_back(new SpeakerClearAction(this));
	this->actions.push_back(new SpeakerListAction(this));

	this->actions.push_back(new SentenceLoadAction(this));
	this->actions.push_back(new SentenceImportFileAction(this));
	this->actions.push_back(new SentenceImportInputAction(this));
	this->actions.push_back(new SentenceClearAction(this));
	this->actions.push_back(new SentenceListAction(this));
	this->actions.push_back(new SentenceSourceListAction(this));

	this->actions.push_back(new TrainingDataBuildAction(this));
	this->actions.push_back(new TrainingDataLoadAction(this));
	this->actions.push_back(new TrainingDataClearAction(this));
	this->actions.push_back(new TrainingDataInfoAction(this));

	this->actions.push_back(new ConfigSetAction(this));
	this->actions.push_back(new ConfigDeleteAction(this));
	this->actions.push_back(new ConfigResetAction(this));
	this->actions.push_back(new ConfigListAction(this));

	this->actions.push_back(new TestRawSentenceImportAction(this));

	this->action_aliases["exit"] = "quit";
	this->action_aliases["?"] = "help";

	this->art.push_back(new SwirlArt(this));
	this->art.push_back(new StaticArt(this));
	this->art.push_back(new PoemArt(this));
}

size_t AIProgram::lexicon_size() {
	return LexiconCountCmd(SqlContext()).execute(nullptr);
}

int AIProgram::run() {
    this->m_model = Get_TinyDnn_Adapter(this);
	auto lexicon_size = this->lexicon_size();

	if (lexicon_size > 0) {
		std::cout << "Creating NN..." << std::endl;
		this->m_model->create_model();
		std::cout << "Done creaiting NN";
		if (this->m_model->has_saved_model()) {
			std::cout << ", loading previously saved model..." << std::endl;
			this->m_model->load_model();
			std::cout << "Done loading past experience." << std::endl;
		} else {
			std::cout << std::endl;
		}

		this->lexicon = new LexiconModel(lexicon_size);
	} else {
		this->lexicon = new LexiconModel(-1);
	}
	this->speakers = new SpeakerModel(this->speaker_capacity(), this->lexicon);	

	this->loop();
	std::cout << std::endl << "Done!" << std::endl;

    return 0;
}

void AIProgram::loop() {
	while (this->should_continue) {
		std::cout << "> ";

		std::string nextCommand;
		getline(std::cin, nextCommand);
		trim(nextCommand);
		if (nextCommand.length() == 0) {
			continue;
		}

		auto command_found = false;
		auto aliased = this->action_aliases.find(nextCommand);
		if (aliased != this->action_aliases.end()) {
			nextCommand = aliased->second;
		}

		for (auto it = this->actions.begin(); it != this->actions.end(); it++) {
			auto label = (*it)->label();
			if (nextCommand == label || (nextCommand.length() == 1 && nextCommand[0] == label[0])) {
				command_found = true;
				(*it)->run();
				break;
			}
		}

		if (!command_found) {
			std::cout << "Invalid command " << nextCommand << std::endl;
		}
	}
}

const char* AIProgram::label() { return "ScriptAI compiled " __DATE__ " at " __TIME__; }

const char* AIProgram::description() { return "ScriptAI is an AI capable of writing scripts."; }