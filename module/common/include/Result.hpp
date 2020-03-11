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


#pragma mark - Result: _OkResult -

/**
 * The Ok result-type.
 * This should be used to construct Results with a present value.
 *
 * ## Example
 * <code>
 * Result<std::string, int> success = Ok("Good job!");
 * </code>
 *
 * @tparam V The value type.
 */
template <typename V>
_OkResult<V> Ok(V value) {
    return _OkResult<V>(value);
};

_OkResult<void> Ok() {
    return _OkResult<void>();
}

template <typename V = void>
class _OkResult {
public:
    using type = std::conditional_t<std::is_void_v<V>, Empty, V>;

public:
    type value;

    /**
     * Creates a new Ok result-type.
     * @param value The value.
     */
    template <typename = std::enable_if_t<!std::is_void_v<V>>>
    _OkResult(type value)
        : value(std::move(value)) {}

    /**
     * Creates a new empty Ok result-type.
     * This only exists when V = void.
     */
    template <typename Enabled>
    _OkResult(std::enable_if_t<std::is_void_v<V>, Enabled> _ = Empty {}) {}
};


#pragma mark - Result: Error -

/**
 * The Error result-type.
 * This should be used to construct Results with an error value.
 *
 * ## Example
 * <code>
 * Result<std::string, int> success = Error(3);
 * </code>
 *
 * @tparam E The error value type.
 */
template <typename E>
_ErrorResult<T> Error(T value) {
    return _ErrorResult<T>(std::move(T));
};

_ErrorResult<void> Error() {
    return _ErrorResult<void>();
}

template <typename E = void>
class _ErrorResult {
public:
    using type = std::conditional_t<std::is_void_v<E>, Empty, E>;

public:
    type value;

    /**
     * Creates a new Error result-type.
     * @param value The error.
     */
    template <typename = std::enable_if_t<!std::is_void_v<E>>>
    _ErrorResult(type value)
        : value(std::move(value)) {}

    /**
     * Creates a new empty Error result-type.
     * This only exists when E = void.
     */
    template <typename Enabled>
    _ErrorResult(std::enable_if_t<std::is_void_v<E>, Enabled> _ = Empty {}) {}
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
     * @param value The Ok result.
     */
    Result(_OkResult<V> value)
        : _value(value.value)
        , _ok(true) {}

    /**
     * Creates a new Result with an Error value.
     * This overload signifies an unsuccessful result.
     *
     * @param value The Error result.
     */
    Result(_ErrorResult<E> value)
        : _error(value.value)
        , _ok(false) {}

    ~Result() {
        if (this->_ok) {
            this->_value.~value_type();
        } else {
            this->_error.~error_type();
        }
    }

    constexpr void operator=(const _ErrorResult<E>& error) {
        this->_ok = false;
        this->_error = error;
    }

    constexpr void operator=(const _OkResult<V>& value) {
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
     * Unwraps the Result.
     * This will throw if the Result is an error.
     *
     * @return The Result's value.
     */
    template <typename U = V>
    constexpr typename std::enable_if_t<!std::is_void_v<U>, const U&> operator*() {
        if (!this->_ok) {
            throw ResultException(ResultException::NOT_ERROR);
        }
        return this->_value;
    }

    /**
     * Unwraps the Result.
     * This will throw if the Result is not an error.
     *
     * @return The Result's error.
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