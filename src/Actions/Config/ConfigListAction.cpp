#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/Commands/Config/ConfigCountCmd.hpp"
#include "../../Data/Commands/Config/ConfigQueryPagedCmd.hpp"

#include <math.h>

using namespace ScriptAI;
using namespace Sql;

ConfigListAction::ConfigListAction(AIProgram *prog): PagedAction(prog, 20) {

}

const char* ConfigListAction::label() {
	return "config list";
}

std::string ConfigListAction::description() {
	return "List the possible configuration options";
}

const char* ConfigListAction::empty_msg() {
	return "Config is empty";
}

size_t ConfigListAction::get_count() {
	return ConfigCountCmd(SqlContext()).execute(nullptr);
}

size_t ConfigListAction::run_paged(size_t start) {
	auto items = ConfigQueryPagedCmd(SqlContext()).execute(paged_position { start, this->page_size });

	size_t i = 0;
	for (auto it = items.begin(); it != items.end(); it++, i++) {
		std::cout << it->handle << ": " << it->asCString() << std::endl;
	}

	return i;
}
