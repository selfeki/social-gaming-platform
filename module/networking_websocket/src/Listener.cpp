#include "Listener.hpp"

#include "BeastContext.hpp"

#include <memory>

using namespace arepa::networking::websocket;
using std::make_shared;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Listener::Listener(const Options& options)
    : _options(options)
    , _context(options)
    , _listener(make_shared<BeastSocketListener>(this->_context))
    , on_accept(this->_listener->on_accept) {
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

void Listener::start() {
    this->_listener->start();
}

void Listener::stop() {
    this->_listener->stop();
}
