#pragma once

#include <exception>
#include <string>
#include <utility>

namespace arepa {

#pragma mark - Result: Empty -
namespace {
    /**
     * An empty class.
     * This is used because we can't have void types.
     */
    class Empty {
    };
}


#pragma mark - Result: ResultException -

/**
 * An exception that is thrown when incorrectly unwrapping a Result.
 */
class ResultException : public std::exception {
public:
    enum Kind {
        NOT_VALUE,
        NOT_ERROR
    };

private:
    Kind _kind;

public:
    ResultException(Kind kind)
        : _kind(kind) {}


    const char* what() {
        if (_kind == NOT_VALUE) {
            return "Attempted to unwrap value of error-type Result.";
        } else {
            return "Attempted to unwrap error of value-type Result.";
        }
    }
};


#pragma mark - Result: Ok -

/**
 * The Ok-Result type.
 * This should be used to construct Results with a present value.
 *
 * ## Example
 * <code>
 * Result<std::string, int> success = Ok<std::string>("Good job!");
 * </code>
 *
 * @tparam V The value type.
 */
template <typename V = void>
class Ok {
public:
    using type = std::conditional_t<std::is_void_v<V>, Empty, V>;

public:
    type value;

    /**
     * Creates a new Ok-Result type.
     * @param value The value.
     */
    template <typename = std::enable_if_t<!std::is_void_v<V>>>
    Ok(type value)
        : value(std::move(value)) {}

    /**
     * Creates a new empty Ok-Result type.
     * This only exists when V = void.
     */
    template <typename Enabled>
    Ok(std::enable_if_t<std::is_void_v<V>, Enabled> _ = Empty {}) {}
};


#pragma mark - Result: Error -

/**
 * The Error-Result type.
 * This should be used to construct Results with an error value.
 *
 * ## Example
 * <code>
 * Result<std::string, int> success = Error<std::string>(3);
 * </code>
 *
 * @tparam E The error value type.
 */
template <typename E = void>
class Error {
public:
    using type = std::conditional_t<std::is_void_v<E>, Empty, E>;

public:
    type value;

    /**
     * Creates a new Error-Result type.
     * @param value The error.
     */
    template <typename = std::enable_if_t<!std::is_void_v<E>>>
    Error(type value)
        : value(std::move(value)) {}

    /**
     * Creates a new empty Error-Result type.
     * This only exists when E = void.
     */
    template <typename Enabled>
    Error(std::enable_if_t<std::is_void_v<E>, Enabled> _ = Empty {}) {}
};


#pragma mark - Result -

/**
 * An alternative error-handling mechanism where you can return a struct that holds either a success value, or an
 * error value.
 *
 * This is similar to Rust's <code>Result<T, E></code> or Haskell's <code>Either V E</code>.
 *
 * ## Example
 * <code>
 *
 * </code>
 *
 * @tparam V The Ok value type.
 * @tparam E The Error value type.
 */
template <typename V, typename E>
class Result {
public:
    using value_type = std::conditional_t<std::is_void_v<V>, Empty, V>;
    using error_type = std::conditional_t<std::is_void_v<E>, Empty, E>;

private:
    bool _ok;
    union {
        value_type _value;
        error_type _error;
    };

public:
    /**
     * Creates a new result with an Ok value.
     * This overload signifies a successful result.
     *
     * @param value The Ok-value.
     */
    Result(Ok<V> value)
        : _value(value.value)
        , _ok(true) {}

    /**
     * Creates a new Result with an Error value.
     * This overload signifies an unsuccessful result.
     *
     * @param value The Error-value.
     */
    Result(Error<E> value)
        : _error(value.value)
        , _ok(false) {}

    ~Result() {
        if (this->_ok) {
            this->_value.~value_type();
        } else {
            this->_error.~error_type();
        }
    }

    constexpr void operator=(const Error<E>& error) {
        this->_ok = false;
        this->_error = error;
    }

    constexpr void operator=(const Ok<V>& value) {
        this->_ok = true;
        this->_value = value;
    }

    /**
     * Checks if the Result was successful.
     * @return True if the Result is success.
     */
    constexpr operator bool() {
        return this->_ok;
    }

    /**
     * Unwraps the Ok-value.
     * This will throw if the Result is an error.
     *
     * @return The Ok-value.
     */
    template <typename U = V>
    constexpr typename std::enable_if_t<!std::is_void_v<U>, const U&> operator*() {
        if (!this->_ok) {
            throw ResultException(ResultException::NOT_ERROR);
        }
        return this->_value;
    }

    /**
     * Unwraps the Error-value.
     * This will throw if the Result is not an error.
     *
     * @return The Error-value.
     */
    template <typename U = E>
    constexpr typename std::enable_if_t<!std::is_void_v<U>, const U&> error() {
        if (this->_ok) {
            throw ResultException(ResultException::NOT_ERROR);
        }
        return this->_error;
    }
};

}