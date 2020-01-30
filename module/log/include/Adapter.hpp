#pragma once

#include "Entry.hpp"

#include <iostream>

namespace arepa::log {

/**
 * A class which provides an interface to store or print log entries.
 * @tparam Collector The class used to collect extra values into a vector.
 */
class Adapter {

#pragma mark - Constructors -
public:
    virtual ~Adapter() = default;

#pragma mark - Virtual -
public:
    /**
     * Processes a log entry.
     * The functionality of this is dependent on the subclass adapter.
     *
     * @param entry The log entry.
     */
    virtual void log(const Entry& entry) = 0;
};

}
