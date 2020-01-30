#include "Adapter.hpp"
#include "Formatter.hpp"

#include "arepa/log/Entry.hpp"

#include <iostream>

using namespace arepa::log::console;
using arepa::log::Entry;
using std::cout;
using std::endl;
using std::lock_guard;
using std::make_unique;
using std::unique_ptr;

Adapter::Adapter()
    : _formatter(make_unique<Formatter>()) {}

void Adapter::use_formatter(std::unique_ptr<Formatter> formatter) {
    this->_formatter = std::move(formatter);
}

void Adapter::process_log(const Entry& entry) {
    // FIXME: Could probably go for something that doesn't require blocking?
    lock_guard guard(this->_mutex);
    cout << this->_formatter->format_entry(entry) << endl;
}
