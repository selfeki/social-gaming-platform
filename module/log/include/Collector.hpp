#pragma once

#include "Entry.hpp"
#include "Level.hpp"

#include <iostream>
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

#pragma mark - Operators -
public:
    /**
     * Collects a value.
     * @param value The value.
     */
    template <typename T>
    Collector& operator<<(const T& value) {
        std::stringstream convert;
        convert << value;
        this->_values.emplace_back(convert.str());
        return *this;
    }
};

}
