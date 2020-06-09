#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"
#include "../../Data/Image/ImageLoader.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesAddCmd.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesQueryPagedCmd.hpp"
#include "../../Data/Commands/TrainingImages/TrainingImagesAddCmd.hpp"

#include "../../Data/Regex.hpp"
#include "../../StackOverflow.hpp"
#include "../../ImageIterator.hpp"

#include <tiny_dnn/io/display.h>

#include <png++-0.2.9/png.hpp>

#include <iostream>
#include <fstream>
#include <string>

using namespace PigAI;
using namespace Sql;


SourceImagesAddLoadSilo::SourceImagesAddLoadSilo(bool& should_continue, size_t* thread_count):
	ThreadedActionSilo(should_continue, thread_count) {}

void SourceImagesAddLoadSilo::process(size_t thread, const PathAndCategory& path) {
	auto img = Load_Image(path.path);
	img->category = path.category;
	this->pass_on(*img);
}

SourceImagesAddSaveSilo::SourceImagesAddSaveSilo(bool& should_continue):
	ThreadedActionSilo(should_continue, nullptr) {}

void SourceImagesAddSaveSilo::process(size_t thread, const ImageData& img) {
	SourceImagesAddCmd(SqlContext()).execute(&img);
	const ImageData img_saved = {
		sqlite3_last_insert_rowid(SqlContext()),
		img.category, img.width, img.height, img.channels,
        img.path, img._data
	};
	this->pass_on(img_saved);
}

SourceImagesAddResizeSilo::SourceImagesAddResizeSilo(bool& should_continue, size_t* thread_count):
	ThreadedActionSilo(should_continue, thread_count) {

}

void SourceImagesAddResizeSilo::process(size_t thread, const ImageData& source) {
	png::image< png::rgb_pixel > image(source.width, source.height);
	for (int y = 0; y < source.height; y++) {
		for (int x = 0; x < source.width; x++) {
			png::rgb_pixel p;
			p.red = source._data[y * source.width * 3 + x * 3 + 0];
			p.green = source._data[y * source.width * 3 + x * 3 + 1];
			p.blue = source._data[y * source.width * 3 + x * 3 + 2];

			image[y][x] = p;
		}
	}
	delete[] source._data;
	image.resize(this->input_width, this->input_width);
	
	const ImageData new_img {
		source.id, source.category,
		this->input_width, this->input_width, this->input_channels,
		source.path,
		new char[this->input_width * this->input_width * this->input_channels],
	};
	this->pass_on(new_img);
}

SourceImagesAddSaveTrainingSilo::SourceImagesAddSaveTrainingSilo(bool& should_continue):
	ThreadedActionSilo(should_continue, nullptr) {
}

void SourceImagesAddSaveTrainingSilo::process(size_t thread, const ImageData& img) {
	TrainingImagesAddCmd(SqlContext()).execute(&img);
}

SourceImagesAddAction::SourceImagesAddAction(AIProgram *prog):
	ThreadedAction(prog, 4),
	items_to_load(this->should_continue, &this->thread_count),
	items_to_save(this->should_continue),
	items_to_resize(this->should_continue, &this->thread_count),
	items_to_save_to_train(this->should_continue) {
	this->items_to_load.pipe_to<ImageData>(&this->items_to_save);
	this->items_to_save.pipe_to<ImageData>(&this->items_to_resize);
	this->items_to_resize.pipe_to<void*>(&this->items_to_save_to_train);
	this->items_to_resize.set_input_properties(_prog->input_width(), _prog->input_channels());
}

void _thread_load_run(SourceImagesAddAction* thiz, size_t thread) { thiz->items_to_load.run(thread); }

void _thread_save_run(SourceImagesAddAction* thiz, size_t thread) { thiz->items_to_save.run(thread); }

void _thread_resize_run(SourceImagesAddAction* thiz, size_t thread) { thiz->items_to_resize.run(thread); }

void _thread_save_train_run(SourceImagesAddAction* thiz, size_t thread) { thiz->items_to_save_to_train.run(thread); }

bool SourceImagesAddAction::try_resolve_category(const std::string& category, int &out_id) {
	auto category_id = std::find(categories().begin(), categories().end(), category);
	if (category_id != categories().cend()) {
		out_id = std::distance(categories().begin(), category_id);
		return true;
	} else {
		return false;
	}
}

size_t SourceImagesAddAction::silo_thread_size(size_t silo) {
	switch (silo) {
		case 1: case 3: return 1;
		default: return this->thread_count;
	}
}

std::thread* SourceImagesAddAction::create_thread(size_t silo, size_t thread) {
	switch (silo) {
		 case 0:
		 return new std::thread(_thread_load_run, this, thread);
		 case 1:
		 return new std::thread(_thread_save_run, this, thread);
		 case 2:
		 return new std::thread(_thread_resize_run, this, thread);
		 case 3:
		 return new std::thread(_thread_save_train_run, this, thread);
	}
	return nullptr;
}

size_t SourceImagesAddAction::sizeof_silo_thread(size_t silo, size_t thread) {
	switch (silo) {
		case 0:
		return this->items_to_load.size(thread);
		case 1:
		return this->items_to_save.size(thread);
		case 2:
		return this->items_to_resize.size(thread);
		case 3:
		return this->items_to_save_to_train.size(thread);
	}
	return 0;
}

const char* SourceImagesAddAction::silo_label(size_t silo) {
	switch (silo) {
		case 0:
		return "load";
		case 1:
		return "save sources";
		case 2:
		return "resize for training";
		case 3:
		return "save for training";
	}
	return "";
}

void SourceImagesAddAction::init_silo(size_t silo) {
	switch (silo) {
		case 0: this->items_to_load.init(); break;
		case 1: this->items_to_save.init(); break;
		case 2: this->items_to_resize.init(); break;
		case 3: this->items_to_save_to_train.init(); break;
	}
}

void SourceImagesAddAction::clean_silo(size_t silo) {
	switch (silo) {
		case 0: this->items_to_load.clean(); break;
		case 1: this->items_to_save.clean(); break;
		case 2: this->items_to_resize.clean(); break;
		case 3: this->items_to_save_to_train.clean(); break;
	}
}