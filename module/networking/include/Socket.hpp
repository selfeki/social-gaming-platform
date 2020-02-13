#pragma once

#include "NetworkException.hpp"

#include <arepa/communication/Signal.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>

namespace arepa::networking {

/**
 * An abstract class representing a remote socket.
 */
class Socket {

#pragma mark - Types -
public:
    typedef std::vector<uint8_t> Data;


#pragma mark - Fields -
private:
#pragma mark - Signals -
public:
    /**
     * A signal for when the data is received from the socket.
     */
    arepa::communication::Signal<const Data&> on_data;

    /**
     * A signal for when the socket is closed.
     */
    arepa::communication::Signal<> on_close;

    /**
     * A signal for when the socket is closed due to an error.
     */
    arepa::communication::Signal<NetworkException> on_error;


#pragma mark - Methods (Private) -
private:
#pragma mark - Constructors -
public:
    Socket() = default;
    virtual ~Socket() = default;

#pragma mark - Methods -
public:
    /**
     * Checks if the socket is still connected.
     * @return True if connected, false otherwise.
     */
    virtual bool is_connected() = 0;

    /**
     * Closes the socket.
     * @param reason If applicable, the reason for closing the socket.
     */
    virtual void close(std::string reason) = 0;

    /**
     * Closes the socket.
     */
    virtual void close() = 0;

    /**
     * Sends data across the socket.
     * @param data The data to send.
     */
    virtual void send(Data data) = 0;
};

}
