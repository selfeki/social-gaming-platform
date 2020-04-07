#pragma once

#include "User.hpp"
#include "Member.hpp"

#include <chrono>

namespace arepa::chat {
class Room;

/**
 * A pointer to a room member.
 */
template <typename T>
class MemberPtr {
    friend Room;

#pragma mark - Types -
public:
    using ptr_type = Member*;
    using value_type = std::shared_ptr<T>;


#pragma mark - Field -
private:
    ptr_type _ptr;


#pragma mark - Constructors -
private:
    MemberPtr(const ptr_type member)
        : _ptr(member) {};

public:
    MemberPtr(const MemberPtr<T>& copy) = default;


#pragma mark - Methods -
public:
    decltype(_ptr->user()) user() const {
        return this->_ptr->user();
    };

    bool is_player() const {
        return this->_ptr->is_player();
    }

    bool is_spectator() const {
        return this->_ptr->is_spectator();
    }

    bool is_disqualified() const {
        return this->_ptr->is_disqualified();
    }

    bool is_waitlisted() const {
        return this->_ptr->is_waitlisted();
    }


#pragma mark - Operators -
public:
    T& operator*() {
        return **this->_ptr;
    }

    T& operator*() const {
        return **this->_ptr;
    }

    T* operator->() {
        return &*this->_ptr->user();
    }

    T* operator->() const {
        return &*this->_ptr->user();
    }

    operator bool() const {    // NOLINT
        return this->_ptr != nullptr;
    }

    explicit operator Member*() const {    // NOLINT
        return this->_ptr;
    }

    bool operator==(const std::shared_ptr<User>& other) const {
        return (*this->_ptr) == other;
    }

    bool operator!=(const std::shared_ptr<User>& other) const {
        return !(*this == other);
    }

    template <typename U>
    bool operator==(const MemberPtr<U>& other) const {
        return *this->_ptr == *other._ptr;
    }

    template <typename U>
    bool operator!=(const MemberPtr<U>& other) const {
        return !(*this == other);
    }

    template <typename U>
    bool operator<(const MemberPtr<U>& other) const {
        return *this->_ptr < *other._ptr;
    }

    template <typename U>
    bool operator>(const MemberPtr<U>& other) const {
        return *this->_ptr > *other._ptr;
    }
};


}
