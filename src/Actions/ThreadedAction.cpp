#include "Actions.hpp"

#include <tiny_dnn/config.h>
#include <tiny_dnn/io/display.h>

#include <iostream>

using namespace PigAI;


ThreadedAction::ThreadedAction(AIProgram *prog, size_t silo_count):
    MenuAction(prog), threads(nullptr), should_continue(true), silo_count(silo_count) {
}

ThreadedAction::~ThreadedAction() {
    this->cleanup();
}

void ThreadedAction::wait_for_threads(tiny_dnn::progress_display* prog) {
	if (this->thread_count > 1 ) {
        for (size_t silo = 0; silo < silo_count; silo++) {
            auto tc = this->silo_thread_size(silo);
            for (size_t thread = 0; thread < tc; thread++) {
                auto last_count = this->sizeof_silo_thread(silo, thread);
                if (last_count > 0) {
                    std::cout << std::endl << std::endl << "Waiting for " << last_count << " items to " << this->silo_label(silo) << " on thread " << (thread + 1) << "/" << tc << "..." << std::endl;
                    prog->restart(last_count);
                    auto try_count = 0;
                    while (last_count > 0) {
                        auto tmp_count = this->sizeof_silo_thread(silo, thread);
                        if (last_count == tmp_count) {
                            try_count++;
                            if (try_count > 100) {
                                std::cerr << "Thread seems to be stuck on " << tmp_count << " after " << try_count << " attempts" << std::endl;
                                try_count = 0;
                            }
                        } else {
                            try_count = 0;
                            (*prog) += last_count - tmp_count;
                            last_count = tmp_count;
                            if (prog->count() > prog->expected_count()) {
                                prog->restart(prog->count());
                            }
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                } else {
                    std::cout << std::endl << "No items left to " << this->silo_label(silo) << " on thread " << (thread + 1) << "/" << tc << "..." << std::endl;
                }
            }
        }
	}
    this->cleanup();
}

void ThreadedAction::cleanup() {
    this->should_continue = false;

	if (thread_count > 1) {
        for (size_t silo = silo_count - 1; silo < silo_count; silo--) {
            auto tc = this->silo_thread_size(silo);
            for (size_t thread = 0; thread < tc; thread++) {
                threads[silo * thread_count + thread]->join();
                delete threads[silo * thread_count + thread];
            }
            this->clean_silo(silo);
        }
        delete[] this->threads;
        this->threads = nullptr;
	}

    this->should_continue = true;
}

void ThreadedAction::ask_thread_count() {
	std::cout << "Thread count: ";
	std::cin >> this->thread_count;
	std::string dummy;
	getline(std::cin, dummy);

	if (thread_count > 1) {
        threads = new std::thread*[silo_count * thread_count];
        for (size_t silo = 0; silo < silo_count; silo++) {
            this->init_silo(silo);
            for (size_t thread = 0; thread < this->silo_thread_size(silo); thread++) {
                threads[silo * thread_count + thread] = this->create_thread(silo, thread);
            }
        }
	}
}