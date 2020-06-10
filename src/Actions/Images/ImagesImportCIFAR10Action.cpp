
#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Regex.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesAddCmd.hpp"
#include "../../Data/Commands/TrainingImages/TrainingImagesAddCmd.hpp"

#include <tiny_dnn/io/display.h>

#include <png++-0.2.9/png.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <regex>

using namespace PigAI;
using namespace Sql;

ImagesImportCIFAR10Action::ImagesImportCIFAR10Action(AIProgram *prog): MenuAction(prog) {

}

const char* ImagesImportCIFAR10Action::label() {
	return "images import cifar10";
}

std::string ImagesImportCIFAR10Action::description() {
	return "Add training images from a CIFAR10 file";
}

void ImagesImportCIFAR10Action::run() {
	std::cout << "Add cifar10 file: ";
	std::string import_path;
	getline(std::cin, import_path);

	std::ifstream ifs(import_path.c_str(), std::ios::in | std::ios::binary);
	if (ifs.fail() || ifs.bad()) {
		std::cerr << "failed to open file: " << import_path << std::endl;
		return;
	}

	auto max_categories = categories().size();
	auto w = _prog->input_width() * _prog->node_scale();

	uint8_t label;
	std::vector<unsigned char> buf(w * w * _prog->input_channels());

	ifs.seekg(0, ifs.end);
	size_t total_count = ifs.tellg() / (w * w * _prog->input_channels() + sizeof(label));
	ifs.seekg(0, ifs.beg);

	tiny_dnn::progress_display prog(total_count);

	SourceImagesAddCmd source_add_cmd(SqlContext());
	TrainingImagesAddCmd training_add_cmd(SqlContext());

	int i = 0, skipped = 0;
	while (ifs.read(reinterpret_cast<char *>(&label), 1)) {
		if (!ifs.read(reinterpret_cast<char *>(&buf[0]), buf.capacity())) break;

		++prog;
		
		if (label >= max_categories) {
			skipped++;
			continue;
		}

		std::stringstream p;
		p << import_path << "/" << i;

		ImageData new_img {
			-1, label,
			w, w, _prog->input_channels(),
			p.str(), (char*)&buf[0]
		};
		source_add_cmd.execute(&new_img);
		
		new_img.id = sqlite3_last_insert_rowid(SqlContext());

		training_add_cmd.execute(&new_img);
		i++;
	}

	std::cout << std::endl;
	if (skipped > 0) {
		std::cout << "Had to skip " << skipped << " images not within the " << max_categories << " categories" << std::endl;
	}
}
