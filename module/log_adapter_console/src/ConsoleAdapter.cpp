#include "ConsoleAdapter.hpp"

#include "Formatter.hpp"
#include "arepa/log/Entry.hpp"

#include <iostream>

using namespace arepa::log::console;
using arepa::log::Entry;
using std::cout;
using std::endl;
using std::lock_guard;
using std::make_unique;
using std::move;
using std::string;
using std::unique_ptr;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

ConsoleAdapter::ConsoleAdapter()
    : _formatter(make_unique<Formatter>()) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

void ConsoleAdapter::use_formatter(unique_ptr<Formatter> formatter) {
    this->_formatter = move(formatter);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Overrides: Adapter -
// ---------------------------------------------------------------------------------------------------------------------

void ConsoleAdapter::log(const Entry& entry) {
    string formatted = this->_formatter->format_entry(entry);

    lock_guard guard(this->_mutex);
    cout << formatted << endl;
}
