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

class Listener {

#pragma mark - Fields -
private:
    const Options _options;
    BeastContext _context;
    std::shared_ptr<BeastSocketListener> _listener;

#pragma mark - Signals -
public:
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
     */
    void start() noexcept(false);

    /**
     * Stops listening for websocket connections.
     */
    void stop();
};

}
