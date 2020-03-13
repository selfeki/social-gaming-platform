#include "ServerLoop.hpp"

#include <chrono>
#include <exception>

using namespace arepa::server;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

ServerLoop::ServerLoop(std::function<void()> tickFunction)
    : _tick(std::move(tickFunction))
    , _running(false)
    , _started(false) {}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

void ServerLoop::_main() {
    while (this->_running) {
        const long long tickDelay = 1000 / this->_tick_rate;
        const auto timestamp = system_clock::now();

        // Run the tick.
        try {
            this->_tick();
        } catch (std::exception& ex) {
            log::fatal("Uncaught exception in server tick.", ex);
            throw;
        }

        // Calculate the time taken.
        auto tickTime = duration_cast<milliseconds>(system_clock::now() - timestamp).count();
        if (tickTime >= tickDelay) {
            log::warning("Running behind!",
                pair("Maximum", tickDelay),
                pair("Actual ", tickTime));

            continue;
        }

        // Wait for the next tick.
        usleep((tickDelay - tickTime) * 1000);
    }
}

void ServerLoop::start() {
    bool yes = false;
    if (!this->_running.compare_exchange_weak(yes, true)) {
        throw std::runtime_error("The server is already running.");
    }

    // Initialize the server.
    yes = false;
    if (this->_started.compare_exchange_weak(yes, true)) {
        // Initialize things here?
    }

    // Start the server.
    log::info("Entering main loop.", pair("Ticks", this->_tick_rate));
    this->_main();
    log::info("Exiting main loop.");
}

void ServerLoop::stop() {
    bool no = true;
    if (this->_running.compare_exchange_weak(no, false)) {
        log::info("Stopping main loop.");
    }
}
