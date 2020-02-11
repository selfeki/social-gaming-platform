#pragma once

#include "WeakCollection.hpp"

#include <algorithm>
#include <boost/iterator/transform_iterator.hpp>
#include <cstdint>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace arepa::collections {

/**
 * A mostly-thread-safe weak vector.
 *
 * A weak vector is a special type of garbage-collected vector which holds shared_ptrs with custom deleters.
 * When one these pointers runs out of references, it will automatically remove itself from the vector.
 *
 * @tparam T The value type.
 */
template <typename T, typename Mutex = std::mutex>
class WeakVector : public WeakCollection<WeakVector<T>, T> {
    friend WeakCollection<WeakVector<T>, T>;

private:
    typedef typename WeakCollection<WeakVector<T>, T>::Container Container;

protected:
    Mutex _lock;
    std::vector<std::shared_ptr<Container>> _collection;

protected:
    bool remove(const Container& container) {
        return this->remove(container.data_ptr);
    }

    template <typename P>
    bool remove(const P& object) {
        std::lock_guard<std::mutex> guard(this->_lock);
        auto pos = std::find_if(std::begin(this->_collection), std::end(this->_collection), [&object](const std::shared_ptr<Container>& rhs) {
            return *rhs == object;
        });

        // If pos == end, it wasn't found.
        if (pos == std::end(this->_collection)) {
            return false;
        }

        // Remove it from the vector.
        (*pos)->collection_ptr = nullptr;
        this->_collection.erase(pos);
        return true;
    }

public:
    WeakVector() {}

    WeakVector(Mutex mutex)
        : _lock(mutex) {}

    ~WeakVector() {
        this->remove_all();    // Required to disassociate objects.
    }

    /**
     * Removes an object from this weak vector.
     * @param object The object to remove.
     * @return True if the object was removed, false if not found.
     */
    bool remove(const std::shared_ptr<T>& object) {
        return this->remove(object);
    }

    /**
     * Removes all objects from this weak vector.
     */
    void remove_all() {
        std::lock_guard<std::mutex> guard(this->_lock);
        for (auto& container : this->_collection) {
            container->collection_ptr = nullptr;
        }
    }

    /**
     * Removes all objects from this weak vector.
     * This is not thread safe.
     */
    inline void unsafe_remove_all() {
        this->_collection.clear();
    }

    /**
     * Constructs a value into this vector.
     * @param args The constructor arguments.
     * @return A shared_ptr to the value.
     */
    template <typename... Ts>
    std::shared_ptr<T> emplace(Ts... args) {
        auto container = this->create_container(args...);

        std::lock_guard<std::mutex> guard(this->_lock);
        this->_collection.push_back(container.first);
        return container.second;
    }

    // TODO: push_back

    /**
     * Counts the number of values in this vector.
     * @return The count.
     */
    [[nodiscard]] size_t size() {
        return this->_collection.size();
    }

    /**
     * Returns the internal mutex object.
     */
    [[nodiscard]] Mutex& _mutex() {    // NOLINT
        return this->_lock;
    }

    /**
     * Gets a transformed version of the begin iterator.
     *
     * WARNING: THIS IS NOT THREAD SAFE! LOCK THE INTERNAL MUTEX BEFORE CALLING THIS.
     *
     * @return
     */
    [[nodiscard]] boost::transform_iterator<std::shared_ptr<T> (*)(std::shared_ptr<Container>), decltype(_collection.begin())> begin() {
        return boost::iterators::make_transform_iterator(std::begin(this->_collection), [](std::shared_ptr<Container> container) {
            return **container;
        });
    }

    /**
     * Gets a transformed version of the end iterator.
     *
     * WARNING: THIS IS NOT THREAD SAFE! LOCK THE INTERNAL MUTEX BEFORE CALLING THIS.
     *
     * @return
     */
    [[nodiscard]] boost::transform_iterator<std::shared_ptr<T> (*)(std::shared_ptr<Container>), decltype(_collection.end())> end() {
        return boost::iterators::make_transform_iterator(std::end(this->_collection), [](std::shared_ptr<Container> container) {
            return **container;
        });
    }
};
}
