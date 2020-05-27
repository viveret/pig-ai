#include "../TrainingData/TrainingDataActions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../SentenceIterator.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/TrainingData/TrainingDataSentenceCountCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>
#include <limits.h>

using namespace ScriptAI;
using namespace Sql;


TrainingDataQuerySilo::TrainingDataQuerySilo(bool& should_continue, size_t* thread_count):
	 ThreadedActionSilo(should_continue, thread_count), querySentences(nullptr) {}

void TrainingDataQuerySilo::init() {
	ThreadedActionSilo<Sql::paged_position, Sql::SentenceModel>::init();
	querySentences = new SentenceQueryPagedCmd*[*this->thread_count];
	for (size_t i = 0; i < *this->thread_count; i++) {
		querySentences[i] = new SentenceQueryPagedCmd(SqlContext());
	}
}

void TrainingDataQuerySilo::clean() {
	for (size_t i = 0; i < *this->thread_count; i++) {
		delete querySentences[i];
	}
	delete[] querySentences;
	ThreadedActionSilo<Sql::paged_position, Sql::SentenceModel>::clean();
}

void TrainingDataQuerySilo::process(size_t thread, const Sql::paged_position& sentence_count) {
	auto items = querySentences[thread]->execute(sentence_count);
	for (auto it = items.begin(); it != items.end(); it++) {
		this->pass_on(*it);
	}
}

TrainingDataBuildSilo::TrainingDataBuildSilo(bool& should_continue, size_t* thread_count, AIProgram *prog):
	 ThreadedActionSilo(should_continue, thread_count), _prog(prog) {}

void TrainingDataBuildSilo::process(size_t thread, const SentenceModel& line) {
	size_t id;
	for (FixedSentenceIterator sit(line); sit.good(); sit.next()) {
		if (this->_prog->lexicon->try_get_id(sit.word, id)) {
			this->pass_on(TrainingDataModel { id, sit.index, line.id });
		}
	}
}

TrainingDataSaveSilo::TrainingDataSaveSilo(bool& should_continue, size_t* thread_count):
	 ThreadedActionSilo(should_continue, thread_count), cmdToken(nullptr) {}

void TrainingDataSaveSilo::init() {
	ThreadedActionSilo<TrainingDataModel, void*>::init();
	cmdToken = new TrainingDataAddWordCmd*[*this->thread_count];
	for (size_t i = 0; i < *this->thread_count; i++) {
		cmdToken[i] = new TrainingDataAddWordCmd(SqlContext());
	}
}

void TrainingDataSaveSilo::clean() {
	for (size_t i = 0; i < *this->thread_count; i++) {
		delete cmdToken[i];
	}
	delete[] cmdToken;
	ThreadedActionSilo<TrainingDataModel, void*>::clean();
}

void TrainingDataSaveSilo::process(size_t thread, const TrainingDataModel& word) {
	this->cmdToken[thread]->execute(word);
}

TrainingDataBuildAction::TrainingDataBuildAction(AIProgram *prog): ThreadedAction(prog, 3),
	items_to_query(this->should_continue, &this->thread_count),
	items_to_build(this->should_continue, &this->thread_count, prog),
	items_to_save(this->should_continue, &this->thread_count) {

	this->items_to_query.pipe_to<TrainingDataModel>(&this->items_to_build);
	this->items_to_build.pipe_to<void*>(&this->items_to_save);
}

const char* TrainingDataBuildAction::label() {
	return "training build";
}

std::string TrainingDataBuildAction::description() {
	return "Build training data from sentences and lexicon for neural network";
}

void TrainingDataBuildAction::run() {
	if (TrainingDataSentenceCountCmd(SqlContext()).execute(nullptr) > 0) {
		std::cout << "Training data exists. Clear?";
		if (this->confirm()) {
			TrainingDataClearAction(this->_prog).run();
		} else {
			return;
		}
	}

	auto sentence_count = SentenceCountCmd(SqlContext()).execute(nullptr);

	this->ask_thread_count();
	
	std::cout << "Converting " << sentence_count << " sentences (" << (sentence_count / this->thread_count) << " per thread) to training data..." << std::endl;
	tiny_dnn::progress_display prog(sentence_count);

	size_t i;
	for (i = 0; i < sentence_count; i += 100, prog += 100) {
		this->items_to_query.push(paged_position { i, 100 });
	}
	this->items_to_query.push(paged_position { i, 100 });

	this->wait_for_threads(&prog);

	std::cout << "Success!" << std::endl;
}

namespace ScriptAI::TrainingDataBuild {
	void _thread_query_run(TrainingDataBuildAction* thiz, size_t thread) { thiz->items_to_query.run(thread); }

	void _thread_build_run(TrainingDataBuildAction* thiz, size_t thread) { thiz->items_to_build.run(thread); }

	void _thread_save_run(TrainingDataBuildAction* thiz, size_t thread) { thiz->items_to_save.run(thread); }
}
using namespace TrainingDataBuild;

size_t TrainingDataBuildAction::silo_thread_size(size_t silo) {
	return this->thread_count;
}

std::thread* TrainingDataBuildAction::create_thread(size_t silo, size_t thread) {
	switch (silo) {
		 case 0:
		 return new std::thread(_thread_query_run, this, thread);
		 case 1:
		 return new std::thread(_thread_build_run, this, thread);
		 case 2:
		 return new std::thread(_thread_save_run, this, thread);
	}
	return nullptr;
}

size_t TrainingDataBuildAction::sizeof_silo_thread(size_t silo, size_t thread) {
	switch (silo) {
		case 0:
		return this->items_to_query.size(thread);
		case 1:
		return this->items_to_build.size(thread);
		case 2:
		return this->items_to_save.size(thread);
	}
	return 0;
}

const char* TrainingDataBuildAction::silo_label(size_t silo) {
	switch (silo) {
		case 0:
		return "query";
		case 1:
		return "build";
		case 2:
		return "save";
	}
	return "";
}

void TrainingDataBuildAction::init_silo(size_t silo) {
	switch (silo) {
		case 0: this->items_to_query.init(); break;
		case 1: this->items_to_build.init(); break;
		case 2: this->items_to_save.init(); break;
	}
}

void TrainingDataBuildAction::clean_silo(size_t silo) {
	switch (silo) {
		case 0: this->items_to_query.clean(); break;
		case 1: this->items_to_build.clean(); break;
		case 2: this->items_to_save.clean(); break;
	}
}