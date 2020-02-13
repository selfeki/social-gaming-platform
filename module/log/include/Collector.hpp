#pragma once

#include "Entry.hpp"
#include "Level.hpp"

#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace arepa::log {

/**
 * A class which collects arbitrary extra values into a vector of strings.
 * This uses a stringstream under the hood.
 */
class Collector {

#pragma mark - Fields -
private:
    std::vector<std::string> _values;

#pragma mark - Methods (Static) -
public:
    /**
     * Collects values into a string vector.
     * @param args The values.
     * @return The vector of collected values.
     */
    template <typename... As>
    static std::vector<std::string> collect(const As&... args) {
        Collector collector;
        (collector << ... << args);
        return collector._values;
    }


#pragma mark - Methods -
    /**
     * Returns a reference to the vector of collected values.
     * @return The collected values.
     */
    const std::vector<std::string>& vec() {
        return this->_values;
    }


#pragma mark - Methods (Protected) -

    /**
     * Appends a value to a stringstream.
     * This depends on the stringstream natively accepting said value.
     *
     * @param convert The stringstream.
     * @param value The value to append.
     */
    template <typename T>
    static void append(std::stringstream& convert, const T& value) {
        convert << value;
    }

    /**
     * Appends an optional value to a stringstream.
     * @param convert The stringstream.
     * @param value The value to append.
     */
    template <typename T>
    static void append(std::stringstream& convert, const std::optional<T>& value) {
        if (value) {
            convert << "Some(";
            append(convert, *value);
            convert << ")";
        } else {
            convert << "None";
        }
    }

    /**
     * Appends an exception to a stringstream.
     * @param convert The stringstream.
     * @param value The exception to append.
     */
    static void append(std::stringstream& convert, const std::exception& value) {
        convert << typeid(value).name();
        convert << ": ";
        convert << value.what();
    }


#pragma mark - Operators -
public:
    /**
     * Collects a value.
     * @param value The value.
     */
    template <typename T>
    Collector& operator<<(const T& value) {
        std::stringstream convert;
        Collector::append(convert, value);
        this->_values.emplace_back(convert.str());
        return *this;
    }

    /**
     * Collects a key-value pair value.
     * @param pair The value pair.
     */
    template <typename K, typename V>
    Collector& operator<<(const std::pair<K, V>& pair) {
        std::stringstream convert;
        Collector::append(convert, pair.first);
        convert << " => ";
        Collector::append(convert, pair.second);
        this->_values.emplace_back(convert.str());
        return *this;
    }
};

}
