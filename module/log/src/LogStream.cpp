#include "LogStream.hpp"

#include <exception>
using namespace arepa::log;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

LogStream::LogStream(std::shared_ptr<Adapter> adapter, std::string module)
    : _adapter(std::move(adapter))
    , _current_module(std::move(module))
    , _current_level(Level::INFO) {}

LogStream::LogStream(std::shared_ptr<Adapter> adapter)
    : _adapter(std::move(adapter))
    , _current_module("global")
    , _current_level(Level::INFO) {}

LogStream::~LogStream() {
    if (this->_current_text.rdbuf()->in_avail() > 0 || !this->_current_values.empty()) {
        this->commit();
    }
}

LogStream::LogStream(const arepa::log::LogStream& copy) {
    this->_current_text.str(copy._current_text.str());
    this->_current_values = copy._current_values;
    this->_current_level = copy._current_level;
    this->_current_module = copy._current_module;
    this->_adapter = copy._adapter;
}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Private) -
// ---------------------------------------------------------------------------------------------------------------------

void LogStream::_reset() {
    this->_current_level = Level::INFO;
    this->_current_text.str("");
    this->_current_values.clear();
}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

void LogStream::commit() {
    this->_adapter->log(Entry(
        this->_current_module,
        this->_current_level,
        this->_current_text.str(),
        std::move(this->_current_values)));

    this->_reset();
}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

LogStream& LogStream::operator<<(Level level) {
    this->_current_level = level;
    return *this;
}

LogStream& LogStream::operator<<(const std::string& message) {
    this->_current_text << message;
    return *this;
}

LogStream& LogStream::operator<<(_Manipulators manipulator) {
    switch (manipulator) {
    case _Manipulators::endl:
        this->commit();
        break;

    default:
        throw std::runtime_error("Unknown LogStream manipulator.");
    }

    return *this;
}