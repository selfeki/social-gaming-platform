#pragma once

#include <cstdint>
#include <functional>
#include <list>
#include <shared_mutex>
#include <vector>

namespace arepa::communication {

/**
 * A class that contains std::function-based signal (event) listeners.
 *
 * IMPORTANT IMPLEMENTATION NOTE:
 * When a listener is attached to a signal, it must be removed if the listener's object is destroyed or moved.
 * Failure to do so will result in UAF and/or memory leaks.
 *
 * @tparam Ts The function parameter types.
 */
template <typename... Ts>
class Signal {

#pragma mark - Types -
public:
    typedef std::function<void(Ts...)> Listener;
    typedef std::uint_fast16_t ListenerID;


protected:
    struct Container {
        Listener listener;
        ListenerID listener_id;

        Container(ListenerID id, Listener listener)
            : listener(listener)
            , listener_id(id) {}

        bool operator==(ListenerID id) {
            return this->listener_id == id;
        }
    };


#pragma mark - Fields -
private:
    ListenerID _next_listener_id;
    std::shared_mutex _mutex;
    std::list<Container> _listeners_once;
    std::vector<Container> _listeners;


#pragma mark - Methods -
public:
    /**
     * Adds a listener.
     *
     * @param listener The listener function.
     *
     * @returns The listener ID.
     */
    ListenerID listen(Listener&& listener) {
        std::unique_lock guard(this->_mutex);

        ListenerID id = (this->_next_listener_id++);
        this->_listeners.push_back(Container(id, listener));

        return id;
    }

    /**
     * Adds a listener for the next time something is emitted.
     * This listener will be removed once called.
     * 
     * @param listener The listener function.
     *
     * @returns The listener ID.
     */
    ListenerID listen_once(Listener&& listener) {
        std::unique_lock guard(this->_mutex);

        ListenerID id = (this->_next_listener_id++);
        this->_listeners_once.push_back(Container(id, listener));

        return id;
    }

    /**
     * Forwards this signal to another signal of the same kind.
     * When this signal emits data, the other signal will also emit the same data.
     *
     * @param signal The signal to forward to.
     * 
     * @return The listener ID.
     */
    ListenerID forward(Signal<Ts...>& signal) {
        return this->listen([&signal](const Ts... args) {
            signal.emit(args...);
        });
    }

    /**
     * Returns the number of non-temporary listeners for the signal.
     * @return The signal count.
     */
    std::size_t listener_count() {
        return this->_listeners.size();
    }

    /**
     * Removes a listener.
     *
     * @param id The listener ID.
     *
     * @return True if the listener was removed, false otherwise.
     */
    bool remove(ListenerID id) {
        std::unique_lock guard(this->_mutex);

        // Try to remove it from the regular listeners.
        {
            auto it = std::remove(std::begin(this->_listeners), std::end(this->_listeners), id);
            if (it != std::end(this->_listeners)) {
                this->_listeners.erase(it, std::end(this->_listeners));
                return true;
            }
        }

        // Try to remove it from the once listeners.
        {
            auto it = std::remove(std::begin(this->_listeners_once), std::end(this->_listeners_once), id);
            if (it != std::end(this->_listeners_once)) {
                this->_listeners_once.erase(it, std::end(this->_listeners_once));
                return true;
            }
        }

        // Can't find it.
        return false;
    }

    /**
     * Removes all listeners.
     * @return The number of removed listeners.
     */
    std::size_t remove_all() {
        std::unique_lock guard(this->_mutex);
        std::size_t removed = this->_listeners_once.size() + this->_listeners.size();

        this->_listeners_once.clear();
        this->_listeners.clear();

        return removed;
    }

    /**
     * Emits a signal to the listeners.
     * @param args The signal data.
     */
    void emit(const Ts&... data) {
        std::vector<Listener> listeners;

        {
            // Copy the listeners to the vector.
            // We use a separate vector to prevent deadlocks from signals that remove themselves.
            std::unique_lock guard(this->_mutex);
            listeners.reserve(this->_listeners_once.size() + this->_listeners.size());

            // Copy once-listeners.
            for (auto listener : this->_listeners_once) {
                listeners.push_back(listener.listener);
            }

            // Copy many-listeners.
            for (auto listener : this->_listeners) {
                listeners.push_back(listener.listener);
            }

            // Clear the once-listeners.
            this->_listeners_once.clear();
        }

        // Call the listeners.
        for (auto listener : listeners) {
            listener(data...);
        }
    }


#pragma mark - Operators -
public:
    /**
     * Adds a listener.
     * This is the same as the listen() method.
     */
    ListenerID operator()(std::function<void(Ts...)> func) {
        return this->listen(std::move(func));
    }

    /**
     * Emits a signal.
     * This is the same as the emit() method.
     *
     * @param data The signal data.
     */
    void operator()(const Ts&... data) {
        this->emit(data...);
    }
};
}
