#pragma once

/**
  * Adds object-enum methods to the class.
  * This converts a class or struct into a fake enum capable of having methods.
  */
#define Enum(__type__)                                                                           \
private:                                                                                         \
    Value _value;                                                                                \
                                                                                                 \
public:                                                                                          \
    __type__() = default;                                                                        \
    constexpr __type__(Value value)                                                              \
        : _value(value) {}                                                                       \
    [[nodiscard]] constexpr operator Value() const { return this->_value; }                      \
    explicit operator bool() = delete;                                                           \
    [[nodiscard]] constexpr bool operator==(__type__ rhs) { return this->_value == rhs._value; } \
    [[nodiscard]] constexpr bool operator!=(__type__ rhs) { return this->_value != rhs._value; }
