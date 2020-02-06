#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>

namespace arepa::communication {

/**
 * A filterable multi-producer single-consumer queue of messages designed for thread-safe access.
 *
 * ALL METHODS IN THIS CLASS CAN BE CONSIDERED THREAD-SAFE UNLESS NOTED OTHERWISE!
 *
 * @tparam T The message type.
 */
template <typename T>
class ChannelSingleQueue {
public:
    /**
     * The message type.
     */
    typedef T Type;

    /**
     * The type for filtering a message.
     * This is defined here for use in other classes.
     */
    typedef std::function<bool(const T&)> Filter;

private:
    mutable std::mutex _lock;
    std::queue<T> _queue;
    std::optional<Filter> _filter;

protected:
    /**
     * Checks to see if a message would be rejected by the filter.
     * If there isn't a filter, this will always return false.
     *
     * @param message The message to check.
     * @return True if the message is rejected, false otherwise.
     */
    bool is_rejected(const T& message) const {
        if (!this->_filter)
            return false;

        Filter filter = *(this->_filter);
        return !filter(message);
    }

public:
    /**
     * Creates a new message queue.
     */
    ChannelSingleQueue()
        : _filter(std::nullopt) {
    }

    /**
     * Creates a new message queue with a filter function.
     *
     * The filter function will be called on the thread that calls `send(T)`, and will determine
     * if the message is copied into this queue.
     * 
     * @param filter_fn The filter function.
     */
    explicit ChannelSingleQueue(Filter filter_fn)
        : _filter(filter_fn) {
    }

    /**
     * Gets the number of messages in the queue.
     *
     * @return A count of the messages.
     */
    size_t count() const {
        std::lock_guard<std::mutex> guard(this->_lock);
        return this->_queue.size();
    }

    /**
     * Moves a message into the queue.
     *
     * @param message The message to send.
     */
    void send(T&& message) {
        // Filter out the message if it isn't accepted by the filter function.
        if (this->is_rejected(message))
            return;

        // Safely move the message into the queue.
        std::lock_guard<std::mutex> guard(this->_lock);
        this->_queue.emplace(std::move(message));
    }

    /**
     * Copies a message into the queue.
     *
     * @param message The message to send.
     */
    void send(const T& message) {
        // Filter out the message if it isn't accepted by the filter function.
        if (this->is_rejected(message))
            return;

        // Safely copy the message into the queue.
        std::lock_guard<std::mutex> guard(this->_lock);
        this->_queue.push(message);
    }

    /**
     * Pops an item off the queue.
     * If the queue is empty, this will return `nullopt`.
     *
     * @return The popped message, or nullopt.
     */
    std::optional<Type> receive() {
        std::lock_guard<std::mutex> guard(this->_lock);
        if (this->_queue.empty())
            return std::nullopt;

        T message = std::move(this->_queue.front());
        this->_queue.pop();
        return message;
    }
};
}
