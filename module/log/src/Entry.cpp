#include "Entry.hpp"

#include <chrono>
#include <string>
#include <vector>

using namespace arepa::log;
using std::string;
using std::vector;
using std::chrono::system_clock;

Entry::Entry(string module, Entry::Level level, Entry::Timestamp timestamp, string message, vector<string> values)
    : module(std::move(module))
    , level(level)
    , timestamp(timestamp)
    , message(std::move(message))
    , values(std::move(values)) {}
