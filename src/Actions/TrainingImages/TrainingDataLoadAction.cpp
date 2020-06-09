#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../ImageIterator.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Regex.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesQueryPagedCmd.hpp"
#include "../../Data/Commands/TrainingImages/TrainingImagesCountCmd.hpp"
#include "../../Data/Commands/TrainingImages/TrainingImageGetCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <iostream>
#include <limits.h>

using namespace PigAI;
using namespace Sql;

TrainingImagesLoadAction::TrainingImagesLoadAction(AIProgram *prog): MenuAction(prog) {

}

const char* TrainingImagesLoadAction::label() {
	return "training load";
}

std::string TrainingImagesLoadAction::description() {
	return "Load training data for neural network";
}

void TrainingImagesLoadAction::run() {
	if (this->_prog->m_model->data_exists()) {
		std::cout << "Training data exists. Clear?";
		if (this->confirm()) {
			this->_prog->m_model->clear_data();
		} else {
			return;
		}
	}
	
	auto total_count = TrainingImagesCountCmd(SqlContext()).execute(nullptr);
	std::cout << "There are " << total_count << " images to train from..." << std::endl;
	TrainingImageGetCmd getCmd(SqlContext());

	size_t start, end;

	std::cout << "Load all";
	if (this->confirm()) {
		start = 0;
		end = total_count;
	} else {
		std::cout << "Start: ";
		std::cin >> start;
		std::cout << "End: ";
		std::cin >> end;
	}

	end = std::min(total_count, end);
	std::vector<tiny_dnn::vec_t> training_in;
	std::vector<tiny_dnn::label_t> training_out;

	tiny_dnn::progress_display prog(end - start);
	std::cout << __FUNCTION__ << std::endl;

	auto in_size = this->_prog->input_channels() * this->_prog->input_width() * this->_prog->input_width() * this->_prog->node_scale() * this->_prog->node_scale();
	TrainingImageProperties props = {
        -1, this->_prog->input_width(), this->_prog->input_channels()
	};
	
	start++;
	end++;
	for (size_t i = start; i < end; i++) {
		++prog;
		props.id = i;

		auto training_data = getCmd.execute(&props);
		for (auto img = training_data.begin(); img != training_data.end(); img++) {
			tiny_dnn::vec_t in_val(in_size);
			for (size_t v = 0; v < in_size; v++) {
				in_val[v] = img->_data[v] / 255.0;
			}
			training_in.push_back(in_val);

			tiny_dnn::label_t out_val(categories().size());
			out_val = img->category;
			training_out.push_back(out_val);
		}
	}

	VecListDAO dao_in(training_in);
	LabelListDAO dao_out(training_out);
	this->_prog->m_model->load_data(&dao_in, &dao_out);
	std::cout << "Success!" << std::endl;
}
