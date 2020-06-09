#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/PagedPosition.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesQueryPagedCmd.hpp"

#include <math.h>

using namespace PigAI;
using namespace Sql;

SourceImagesListAction::SourceImagesListAction(AIProgram *prog): PagedAction(prog, 10) {

}

const char* SourceImagesListAction::label() {
	return "source images list";
}

std::string SourceImagesListAction::description() {
	return "List the neural network's source images";
}

const char* SourceImagesListAction::empty_msg() {
	return "Source image list is empty";
}

size_t SourceImagesListAction::get_count() {
	return SourceImagesCountCmd(SqlContext()).execute(nullptr);
}

size_t SourceImagesListAction::run_paged(size_t start) {
	auto items = SourceImagesQueryPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t i = 0;
	for (auto it = items.begin(); it != items.end(); it++, i++) {
		std::cout << (*it).id << "-" << (*it).category << ") ";
		std::cout << (*it).path << ": " << (*it).width << std::endl;
	}

	return i;
}
