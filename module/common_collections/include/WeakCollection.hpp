#pragma once

#include <algorithm>
#include <memory>
#include <utility>

namespace arepa::collections {

/**
 * A base template class for creating weak collections.
 *
 * Weak collections, in this context, are collections which store refcounted objects.
 * If the number of references to the object reach zero, the collection will automatically remove the object.
 *
 * @tparam Derived The deriving class.
 * @tparam T The collection value type.
 */
template <typename Derived, typename T>
class WeakCollection {
protected:
    /**
     * A heap-allocated container that holds a weak pointer to contained object,
     * and a raw pointer back to the collection that it's associated with.
     */
    struct Container {
        std::weak_ptr<T> data_ptr;
        WeakCollection<Derived, T>* collection_ptr;

        bool operator==(const std::shared_ptr<T>& rhs) const {
            return this->data_ptr.lock() == rhs;
        }

        bool operator==(const Container* rhs) const {
            return this == rhs;
        }

        std::shared_ptr<T> operator*() const {
            return this->data_ptr.lock();
        }

        /**
         * Disassociates the container (and contained object) from the collection.
         * This will set the collection pointer to nullptr.
         */
        void disassociate() {
            if (this->collection_ptr == nullptr)
                return;

            auto collection = static_cast<Derived*>(this->collection_ptr);
            this->collection_ptr = nullptr;
            collection->remove(this);
        }
    };

    /**
     * A std::shared_ptr deleter that will disassociate an object (and its container) from the collection once there
     * are no more references to it.
     */
    struct Deleter {
        std::shared_ptr<Container> container;

        Deleter(std::shared_ptr<Container> container)
            : container(container) {}

        void operator()(T* ptr) const {
            this->container->disassociate();
            delete ptr;
        }
    };

    /**
     * Constructs a new T and moves it into a container.
     * @return The container, and a singular shared pointer to keep it from being deleted.
     */
    template <typename... As>
    std::pair<std::shared_ptr<Container>, std::shared_ptr<T>> create_container(As... args) const {
        return this->create_container(std::make_unique<T>(args...));
    }

    /**
     * Creates a new container from a moved unique pointer.
     * @param ptr The unique pointer.
     * @return The container, and a singular shared pointer to keep it from being deleted.
     */
    std::pair<std::shared_ptr<Container>, std::shared_ptr<T>> create_container(std::unique_ptr<T> ptr) const {
        auto container = std::make_shared<Container>();
        auto object = std::shared_ptr<T>(ptr.release(), Deleter(container));

        container->collection_ptr = const_cast<WeakCollection<Derived, T>*>(this);
        container->data_ptr = object;

        return std::pair(container, object);
    }

    /**
     * Creates a new container from a rvalue.
     * @param ptr The unique pointer.
     * @return The container.
     */
    std::pair<std::shared_ptr<Container>, std::shared_ptr<T>> create_container(T&& ref) const {
        auto container = std::make_shared<Container>();
        auto object = std::shared_ptr<T>(std::move(ref), Deleter(container));

        container->collection_ptr = this;
        container->data_ptr = object;

        return std::pair(container, object);
    }
};
}