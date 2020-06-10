#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Commands/Model/PagedPosition.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesQueryPagedCmd.hpp"

#include <math.h>

using namespace PigAI;
using namespace Sql;

ImagesListAction::ImagesListAction(AIProgram *prog): PagedAction(prog, 10) {

}

const char* ImagesListAction::label() {
	return "images list";
}

std::string ImagesListAction::description() {
	return "List the neural network's images";
}

const char* ImagesListAction::empty_msg() {
	return "Image list is empty";
}

size_t ImagesListAction::get_count() {
	return SourceImagesCountCmd(SqlContext()).execute(nullptr);
}

size_t ImagesListAction::run_paged(size_t start) {
	auto items = SourceImagesQueryPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t i = 0;
	for (auto it = items.begin(); it != items.end(); it++, i++) {
		std::cout << (*it).id << "-" << (*it).category << ") ";
		std::cout << (*it).path << ": " << (*it).width << std::endl;
	}

	return i;
}
