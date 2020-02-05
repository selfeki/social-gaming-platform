#pragma once

#include "BeastContext.hpp"
#include "BeastSocketListener.hpp"
#include "Options.hpp"

#include <arepa/networking/Options.hpp>
#include <arepa/networking/Socket.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <cstdint>
#include <memory>

namespace arepa::networking::websocket {

/**
 * A class which acts as a connection listener.
 * This will accept remote websocket connections and construct a Socket objects for each connection.
 */
class Listener {

#pragma mark - Fields -
private:
    const Options _options;
    BeastContext _context;
    std::shared_ptr<BeastSocketListener> _listener;


#pragma mark - Signals -
public:
    /**
     * A signal for when a new socket connection is established.
     */
    arepa::communication::Signal<std::shared_ptr<arepa::networking::Socket>>& on_accept;


#pragma mark - Constructors -
public:
    explicit Listener(const Options& options) noexcept(false);
    ~Listener() = default;


#pragma mark - Methods (Protected) -
protected:
#pragma mark - Methods -
public:
    /**
     * Starts listening for websocket connections.
     * This object should not be moved after this method is called.
     */
    void start() noexcept(false);

    /**
     * Stops listening for websocket connections.
     */
    void stop();
};

}
