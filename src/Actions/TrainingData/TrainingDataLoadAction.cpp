#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../SentenceIterator.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/LexiconModel.hpp"
#include "../../Data/Commands/Model/TrainingDataModel.hpp"
#include "../../Data/Commands/Sentences/SentenceQueryPagedCmd.hpp"
#include "../../Data/Commands/TrainingData/TrainingDataSentenceCountCmd.hpp"
#include "../../Data/Commands/TrainingData/TrainingDataSentenceGetCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>
#include <limits.h>

using namespace ScriptAI;
using namespace Sql;

TrainingDataLoadAction::TrainingDataLoadAction(AIProgram *prog): MenuAction(prog) {

}

const char* TrainingDataLoadAction::label() {
	return "training load";
}

std::string TrainingDataLoadAction::description() {
	return "Load training data for neural network";
}

void TrainingDataLoadAction::run() {
	if (this->_prog->m_model->data_exists()) {
		std::cout << "Training data exists. Clear?";
		if (this->confirm()) {
			this->_prog->m_model->clear_data();
		} else {
			return;
		}
	}
	
	auto sentence_count = TrainingDataSentenceCountCmd(SqlContext()).execute(nullptr);
	std::cout << "There are " << sentence_count << " sentences to train from..." << std::endl;
	SentenceQueryPagedCmd querySentences(SqlContext());

	TrainingDataSentenceGetCmd getCmd(SqlContext());

	size_t start, end;
	std::cout << "Start: ";
	std::cin >> start;
	std::cout << "End: ";
	std::cin >> end;

	end = std::min(sentence_count, end);
	tiny_dnn::tensor_t training_in;
	tiny_dnn::tensor_t training_out;

	tiny_dnn::progress_display prog(end - start);
	std::cout << __FUNCTION__ << std::endl;
	
	for (size_t i = start; i < end; ) {
		auto items = querySentences.execute(paged_position { i, 100 });
		prog += items.size();

		for (auto it = items.begin(); it != items.end(); it++) {
			i = it->id;
			
			auto training_data = getCmd.execute(i);
			auto last_word = training_data.begin();
			auto word = last_word;
			word++;
			for (; word != training_data.end(); word++) {
				tiny_dnn::vec_t in_val(this->_prog->lexicon_size());
				in_val[last_word->feature_index] = 1;
				training_in.push_back(in_val);

				tiny_dnn::vec_t out_val(this->_prog->lexicon_size());
				out_val[word->feature_index] = 1;
				training_out.push_back(out_val);
			}
		}
	}

	TensorDao dao_in(training_in);
	TensorDao dao_out(training_out);
	this->_prog->m_model->load_data(&dao_in, &dao_out);
	std::cout << "Success!" << std::endl;
}
