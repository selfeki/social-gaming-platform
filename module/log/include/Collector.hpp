#pragma once

#include "Entry.hpp"
#include "Level.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace arepa::log {

/**
 * A class which collects the extra values provided by a log function into a vector of strings.
 * This uses a stringstream under the hood.
 */
class Collector {
protected:
    std::vector<std::string> collected;

public:
    template <typename T>
    Collector& operator<<(T value) {
        std::stringstream convert;
        convert << value;
        this->collected.emplace_back(convert.str());
        return *this;
    }

    const std::vector<std::string>& vec() {
        return this->collected;
    }
};

}
