#include "Program.hpp"
#include "AIModelAdapter.hpp"
#include "MenuAction.hpp"

#include "Actions/Actions.hpp"

#include "StackOverflow.hpp"

#include <iostream>
#include <cctype>

using namespace PigAI;


AIProgram::AIProgram(): should_continue(true), m_model(nullptr) {
	this->actions.push_back(new QuitAction(this));
	this->actions.push_back(new HelpAction(this));
	this->actions.push_back(new TestAction(this));
	this->actions.push_back(new TrainAction(this));
	this->actions.push_back(new DaemonAction(this));

	this->actions.push_back(new ModelInitAction(this));
	this->actions.push_back(new ModelRevertAction(this));
	this->actions.push_back(new ModelSaveAction(this));
	this->actions.push_back(new ModelSaveAsAction(this));
	this->actions.push_back(new ModelLoadAction(this));
	this->actions.push_back(new ModelDeleteSavedAction(this));
	this->actions.push_back(new ModelDeleteAction(this));
	this->actions.push_back(new ModelResetWeightsAction(this));

	this->actions.push_back(new ImagesImportFileAction(this));
	this->actions.push_back(new ImagesImportFolderAction(this));
	this->actions.push_back(new ImagesImportCIFAR10Action(this));
	this->actions.push_back(new ImagesClearAction(this));
	this->actions.push_back(new ImagesListAction(this));

	this->actions.push_back(new TrainingImagesLoadAction(this));

	this->actions.push_back(new ConfigSetAction(this));
	this->actions.push_back(new ConfigDeleteAction(this));
	this->actions.push_back(new ConfigResetAction(this));
	this->actions.push_back(new ConfigListAction(this));

	this->action_aliases["exit"] = "quit";
	this->action_aliases["?"] = "help";
}

int AIProgram::run() {
    this->m_model = Get_TinyDnn_Adapter(this);

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

const char* AIProgram::label() { return "PigAI compiled " __DATE__ " at " __TIME__; }

const char* AIProgram::description() { return "PigAI is an AI capable of putting pigs in pens."; }