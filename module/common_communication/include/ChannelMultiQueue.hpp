#pragma once

#include "ChannelSingleQueue.hpp"

#include <arepa/collections/WeakVector.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace arepa::communication {

/**
 * A multiple-producer multiple-consumer queue of messages designed for thread-safe access.
 * This is based on the idea of creating multiple SPSC queues, and sending messages to all of them.
 *
 * ALL METHODS IN THIS CLASS CAN BE CONSIDERED THREAD-SAFE UNLESS NOTED OTHERWISE!
 *
 * THIS CLASS DOES NOT HAVE A `receive` METHOD. USE `create_recevier` TO CREATE A MPSC QUEUE TO READ FROM!
 * 
 * @tparam T The message type.
 */
template <typename T>
class ChannelMultiQueue {
public:
    /**
     * A type alias for a consumer of this type.
     */
    typedef ChannelSingleQueue<T> Consumer;

    /**
     * A type alias for filters.
     */
    typedef typename ChannelSingleQueue<T>::Filter Filter;

private:
    arepa::collections::WeakVector<Consumer> _queues;

public:
    /**
     * Counts the number of live consumers.
     *
     * @return A count of the messages.
     */
    size_t count_consumers() {
        return this->_queues.size();
    }

    /**
     * Creates a new filtered consumer queue.
     * This will forward any producer messages to the consumer queue.
     *
     * If the consumer queue runs out of references, it will automatically remove itself from the MPMC channel.
     *
     * @note `filter_fn` Is called on a producer thread, and should be a pure function to remain thread-safe.
     * @param filter_fn A function which determines what messages should be accepted into the queue.
     *
     * @return A shared pointer to the newly-created queue.
     */
    std::shared_ptr<Consumer> create_consumer(const Filter filter_fn) {
        return this->_queues.emplace(filter_fn);
    }

    /**
     * Creates a new consumer queue.
     * This will forward any producer messages to the consumer queue.
     *
     * If the consumer queue runs out of references, it will automatically remove itself from the MPMC channel.
     *
     * @return A shared pointer to the newly-created queue.
     */
    std::shared_ptr<Consumer> create_consumer() {
        return this->_queues.emplace();
    }

    /**
     * Sends a message to all consumer queues.
     * This will call the `MessageQueue.send(const Message&)` method for each queue.
     *
     * @param message The message to receive.
     */
    void send(const T& message) {
        std::lock_guard<std::mutex> guard(this->_queues._mutex());
        for (auto queue : this->_queues) {
            queue->send(message);
        }
    }
};
}
