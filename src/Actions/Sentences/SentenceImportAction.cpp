#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Sentences/SentenceImportCmd.hpp"
#include "../../Data/Commands/Sentences/SentenceSourceGetCmd.hpp"

#include "../../Data/Regex.hpp"
#include "../../StackOverflow.hpp"
#include "../../SentenceIterator.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace ScriptAI;
using namespace Sql;

PartialSentenceImportParser::PartialSentenceImportParser():
	is_sentence_done(false), add_part(true), assign_to_speaker(true) {
}

void PartialSentenceImportParser::process(SentenceImportParseSilo *silo, size_t thread, const std::string& line) {
	speaker.clear();
	fixed_sentence.clear();
	this->is_sentence_done = false;
	this->add_part = true;
	this->assign_to_speaker = true;
	for (RawSentenceIterator it(line); it.good(); it.next()) {
		if (it.word.length() == 1) {
			auto ch = it.word[0];
			if (ch == '\t') {
				this->add_part = false;
				this->is_sentence_done = true;
			} else if (ch == '.' || ch == '!' || ch == '?' || ch == ';') {
				this->is_sentence_done = true;
			}
		}

		if (this->add_part) {
			fixed_sentence << it.word << ' ';
		} else {
			this->add_part = true;
		}

		if (this->is_sentence_done) {
			if (this->sentence_done(silo, line)) {
				return;
			}
			this->is_sentence_done = false;
		}
	}

	if (!this->is_sentence_done && fixed_sentence.str().length() > 0) {
		this->sentence_done(silo, line);
	}
}

bool PartialSentenceImportParser::sentence_done(SentenceImportParseSilo *silo, const std::string& line) {
	auto fixed_str = fixed_sentence.str();
	trim(fixed_str);

	if (this->assign_to_speaker) {
		speaker = fixed_str;
		this->assign_to_speaker = false;
	} else if (speaker.length() == 0) {
		std::cout << "Speaker not found (format: speaker\\ttext): " << line << std::endl;
		return true;
	} else {
		silo->pass_on(PartialSentenceSaveItem { speaker, fixed_str });
		this->assign_to_speaker = true;
	}
	fixed_sentence.str("");
	fixed_sentence.clear();
	return false;
}

SentenceImportParseSilo::SentenceImportParseSilo(bool& should_continue, size_t* thread_count):
	ThreadedActionSilo(should_continue, thread_count) {}

void SentenceImportParseSilo::init() {
	ThreadedActionSilo::init();
	this->parser = new PartialSentenceImportParser[*this->thread_count];
}

void SentenceImportParseSilo::clean() {
	delete[] this->parser;
	ThreadedActionSilo::clean();
}

void SentenceImportParseSilo::process(size_t thread, const std::string& line) {
	this->parser[thread].process(this, thread, line);
}

SentenceImportSaveSilo::SentenceImportSaveSilo(bool& should_continue, size_t& source_id):
	ThreadedActionSilo(should_continue, nullptr), source_id(source_id) {}

void SentenceImportSaveSilo::process(size_t thread, const PartialSentenceSaveItem& line) {
	SentenceImportCmd(SqlContext()).execute(SentenceModel { 0, source_id, line.speaker, line.what });
}

SentenceImportAction::SentenceImportAction(AIProgram *prog):
	ThreadedAction(prog, 2),
	items_to_parse(this->should_continue, &this->thread_count),
	items_to_save(this->should_continue, this->source_id) {
	this->items_to_parse.pipe_to<void*>(&this->items_to_save);
}

void _thread_parse_run(SentenceImportAction* thiz, size_t thread) { thiz->items_to_parse.run(thread); }

void _thread_save_run(SentenceImportAction* thiz, size_t thread) { thiz->items_to_save.run(thread); }

size_t SentenceImportAction::silo_thread_size(size_t silo) {
	switch (silo) {
		case 1: return 1;
		default: return this->thread_count;
	}
}

std::thread* SentenceImportAction::create_thread(size_t silo, size_t thread) {
	switch (silo) {
		 case 0:
		 return new std::thread(_thread_parse_run, this, thread);
		 case 1:
		 return new std::thread(_thread_save_run, this, thread);
	}
	return nullptr;
}

size_t SentenceImportAction::sizeof_silo_thread(size_t silo, size_t thread) {
	switch (silo) {
		case 0:
		return this->items_to_parse.size(thread);
		case 1:
		return this->items_to_save.size(thread);
	}
	return 0;
}

const char* SentenceImportAction::silo_label(size_t silo) {
	switch (silo) {
		case 0:
		return "parse";
		case 1:
		return "save";
	}
	return "";
}

void SentenceImportAction::init_silo(size_t silo) {
	switch (silo) {
		case 0: this->items_to_parse.init(); break;
		case 1: this->items_to_save.init(); break;
	}
}

void SentenceImportAction::clean_silo(size_t silo) {
	switch (silo) {
		case 0: this->items_to_parse.clean(); break;
		case 1: this->items_to_save.clean(); break;
	}
}

void SentenceImportAction::set_source(std::string& path) {
	SentenceSourceImportCmd(SqlContext()).execute(SentenceSourceModel { 0, path });
	this->source_id = SentenceSourceGetCmd(SqlContext()).execute(path).id;
}


void SentenceImportAction::set_source(const char* path) {
	SentenceSourceImportCmd(SqlContext()).execute(SentenceSourceModel { 0, path });
	this->source_id = SentenceSourceGetCmd(SqlContext()).execute(path).id;
}
