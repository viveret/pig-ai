#ifndef SCRIPTAI_ThreadedActionSilo_H
#define SCRIPTAI_ThreadedActionSilo_H

#include "../queue.hpp"
#include <thread>

namespace ScriptAI {
    template <typename TIn, typename TOut>
    class ThreadedActionSilo {
        protected:
        bool& should_continue;
        const size_t *thread_count;
        size_t _next_thread;
        ThreadedActionSilo<TOut, void*>* _next;
        concurrent_queue<TIn> *_items;

        public:
        ThreadedActionSilo(bool& should_continue, const size_t* thread_count):
            should_continue(should_continue), thread_count(thread_count), _items(nullptr), _next_thread(0) {

        }
        ~ThreadedActionSilo() {
            this->clean();
        }

        template <typename TOutNext>
        inline void pipe_to(ThreadedActionSilo<TOut, TOutNext>* next) {
            this->_next = (ThreadedActionSilo<TOut, void*>*)next;
        }

        inline size_t next_thread() {
            return _next_thread = (_next_thread + 1) % *thread_count;
        }

        inline bool is_single_threaded() { return thread_count == nullptr; }
        
        inline void push(const TIn& val) { !is_single_threaded() ? (*thread_count > 1 ? _items[next_thread()].push(val) : process(next_thread(), val)) : _items[0].push(val); }
        inline bool try_pop(size_t thread, const TIn& val) { return _items[thread].try_pop(val); }
        inline int size(size_t thread) { return _items[thread].size(); }

        inline void pass_on(const TOut& to_pass_on) {
            if (this->_next) {
                this->_next->push(to_pass_on);
            }
        }

        virtual void init() {
            if (!this->is_single_threaded()) {
                if (*thread_count > 1) {
                    this->_items = new concurrent_queue<TIn>[*this->thread_count]();
                }
            } else {
                this->_items = new concurrent_queue<TIn>();
            }
        }

        virtual void clean() {
            if (this->_items) {
                if (is_single_threaded()) {
                    delete this->_items;
                } else {
                    delete[] this->_items;
                }
                this->_items = nullptr;
            }
        }

        virtual void process(size_t thread, const TIn& to_do) = 0;

        void run(size_t thread) {
            TIn to_do;
            while (this->should_continue) {
                while (this->_items[thread].try_pop(to_do)) {
                    this->process(thread, to_do);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    };
}


#endif