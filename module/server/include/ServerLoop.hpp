#pragma once

#include <arepa/log/Log.hpp>

#include <atomic>
#include <functional>

namespace arepa::server {

/**
 * A broad that ties together all the components that make up a server.
 * This is in charge of handling connections and delegating events.
 */
class ServerLoop : Log("ServerLoop") {
    static constexpr size_t DEFAULT_TICK_RATE = 10;


#pragma mark - Fields -
private:
    size_t _tick_rate = DEFAULT_TICK_RATE;
    std::function<void()> _tick;
    std::atomic<bool> _running;
    std::atomic<bool> _started;


#pragma mark - Constructors -
public:
    explicit ServerLoop(std::function<void()> tickFunction);
    virtual ~ServerLoop() = default;

#pragma mark - Methods -
public:
    /**
     * Runs the server main loop.
     * This should not return until shutdown.
     */
    void start();

    /**
     * Stops the server main loop.
     */
    void stop();
};
}
