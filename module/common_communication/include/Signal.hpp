#pragma once

#include <cstdint>
#include <functional>
#include <list>
#include <shared_mutex>
#include <vector>

namespace arepa::communication {

/**
 * A class that contains std::function-based signal (event) listeners.
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

        bool operator==(const ListenerID& id) {
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
        this->_listeners.emplace(id, std::forward<Listener>(listener));

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
        this->_listeners_once.emplace(id, std::forward<Listener>(listener));

        return id;
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
            auto find = std::find(std::begin(this->_listeners), std::end(this->_listeners), id);
            if (find != std::end(this->_listeners)) {
                this->_listeners.erase(find);
                return true;
            }
        }

        // Try to remove it from the once listeners.
        {
            auto find = std::find(std::begin(this->_listeners_once), std::end(this->_listeners_once), id);
            if (find != std::end(this->_listeners_once)) {
                this->_listeners.erase(find);
                return true;
            }
        }

        // Can't find it.
        return false;
    }

    /**
     * Emits a signal to the listeners.
     * @param args The listener arguments.
     */
    void emit(Ts&&... args) {
        std::unique_lock guard(this->_mutex);

        // Emit to once-listeners.
        for (auto listener : this->_listeners_once) {
            listener(std::forward<Ts>(args)...);
        }

        // Emit to many-listeners.
        for (auto listener : this->_listeners) {
            listener(std::forward<Ts>(args)...);
        }

        // Clear once-listeners.
        this->_listeners_once.clear();
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
};
}
