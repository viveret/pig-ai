#include "Actions.hpp"

#include <iostream>

using namespace PigAI;

PagedAction::PagedAction(AIProgram *prog, int page_size): MenuAction(prog), page_size(page_size) {
}

void PagedAction::run() {
    auto count = this->get_count();
    int start = 0;
    bool continue_query = true;
    
    if (count == 0) {
        std::cout << this->empty_msg() << std::endl;
        return;
    }

    while (continue_query) {
        auto item_size = this->run_paged(start);

        if (item_size < page_size) {
            continue_query = false;
        } else if (start + page_size >= count) {
            continue_query = false;
        } else {
            std::cout << "Continue?";
            continue_query = this->confirm();
        }

        start += page_size;
    }
}