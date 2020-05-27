#ifndef PRODUCER_CONSUMER_QUEUE_HPP_
#define PRODUCER_CONSUMER_QUEUE_HPP_


// Based on code from http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
// Original version by Anthony Williams
// Modifications by Michael Anderson -- https://gist.github.com/482342
// Modifications by Viveret Steele -- https://gist.github.com/viveret/323d1cadf57468bd73bc97c3b952c124


#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>

template<typename Data>
class concurrent_queue
{
private:
    std::deque<Data> the_queue;
    mutable std::mutex the_mutex;
    std::condition_variable the_condition_variable;
    bool is_canceled;

public:
    concurrent_queue() : the_queue(), the_mutex(), the_condition_variable(), is_canceled(false) {}

    struct Canceled{};

    void push(Data const& data)
    {
        {
            std::scoped_lock lock(the_mutex);
            if (is_canceled) throw Canceled();
            the_queue.push_back(data);
        }
        the_condition_variable.notify_one();
    }

    bool empty() const
    {
        std::scoped_lock lock(the_mutex);
        if (is_canceled) throw Canceled();
        return the_queue.empty();
    }

    int size() const
    {
        std::scoped_lock lock(the_mutex);
        if (is_canceled) throw Canceled();
        return the_queue.size();
    }

    bool try_pop(Data& popped_value)
    {
        std::scoped_lock lock(the_mutex);
        if (is_canceled) throw Canceled();
        if(the_queue.empty())
        {
            return false;
        }

        popped_value=the_queue.front();
        the_queue.pop_front();
        return true;
    }

    void wait_and_pop(Data& popped_value)
    {
        std::scoped_lock lock(the_mutex);

        while(the_queue.empty() && !is_canceled)
        {
            the_condition_variable.wait(lock);
        }
        if (is_canceled) throw Canceled();

        popped_value=the_queue.front();
        the_queue.pop_front();
    }

    /**Wait and retrieve all data in this queue.
     *
     * @param dest Destination queue that will be emptied and then will
     * receive all data contained in this queue.
     *
     * @warning Previous contents of @c dest are discarded.
     *
     */
    void wait_and_take_all(std::deque<Data>& dest)
    {
        std::scoped_lock lock(the_mutex);

        while(the_queue.empty() && !is_canceled)
        {
            the_condition_variable.wait(lock);
        }
        if (is_canceled) throw Canceled();

        dest.clear();
        dest.swap(the_queue);
    }

    void cancel()
    {
       std::scoped_lock lock(the_mutex);
       if (is_canceled) throw Canceled();
       is_canceled = true;
       lock.unlock();
       the_condition_variable.notify_all();
    }

};


#endif /* PRODUCER_CONSUMER_QUEUE_HPP_ */
