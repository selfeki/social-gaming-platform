#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace arepa::networking {

/**
 * Network configuration options.
 */
class Options {

#pragma mark - Constants -
public:
    static const size_t DEFAULT_THREAD_COUNT = 4;


#pragma mark - Fields -
public:
    /**
     * The number of threads to allocate for async io.
     * This will default to 4 if nothing is provided.
     */
    std::optional<size_t> threads;

    /**
     * The network interface address to bind to.
     * This will default to all interfaces if nothing is provided.
     */
    std::optional<std::string> bind_address;

    /**
     * The network port to bind to.
     */
    uint16_t bind_port;
};

}
