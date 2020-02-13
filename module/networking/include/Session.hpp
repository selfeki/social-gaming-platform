#pragma once

#include "SessionToken.hpp"

#include <boost/uuid/uuid.hpp>
#include <optional>

namespace arepa::networking {
namespace {
    /**
    * The base class for client sessions.
    */
    class SessionBase {
    };

    /**
    * The base class for client sessions.
    * This variant stores additional data that can be tied to the specific session (e.g. auth information).
    *
    * @tparam Data The additional data.
    */
    template <typename Data>
    class SessionBaseWithData {
    private:
        Data _data;

    public:
        /**
         * Constructs a session and its associated data.
         * @param args The arguments used to construct the data.
         */
        template <typename... As>
        SessionBaseWithData(As... args)
            : _data(Data(args...)) {}

        /**
         * Constructs a session with data.
         * @param data The session data.
         */
        SessionBaseWithData(Data&& data)
            : _data(std::forward<Data>(data)) {}

        /**
         * Gets a reference to the additional data.
         *
         * @return The data.
         */
        [[nodiscard]] const Data& data() const {
            return this->_data;
        }

        /**
         * Gets a mutable reference to the additional data.
         *
         * @return The data.
         */
        [[nodiscard]] Data& data() {
            return this->_data;
        }
    };
}

/**
 * An established client session.
 *
 * @tparam Data An optional type for storing additional data.
 */
template <typename Data = void>
class Session : public std::conditional<std::is_void<Data>::value, SessionBase, SessionBaseWithData<Data>>::type {

#pragma mark - Fields -
private:
    SessionToken _token;


#pragma mark - Constructors -
public:
    /**
     * No default constructor.
     */
    Session() = delete;

    /**
     * Creates a new session.
     *
     * @param token The session token.
     * @param args The session data constructor arguments.
     */
    template <typename... As>
    Session(const SessionToken& token, As... args)
        : std::conditional<std::is_void<Data>::value, SessionBase, SessionBaseWithData<Data>>::type(args...)
        , _token(token) {}


#pragma mark - Methods -
public:
    /**
     * Gets a reference to the session token.
     * @return The session token.
     */
    [[nodiscard]] const SessionToken& token() const {
        return this->_token;
    }

    /**
     * Gets a reference to the session ID.
     * @return The session ID.
     */
    [[nodiscard]] const SessionToken::Id& id() const {
        return this->_token.id();
    }


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const SessionToken::Id& id) {
        return this->_token.id() == id;
    }

    [[nodiscard]] bool operator==(const SessionToken& token) {
        return this->_token == token;
    }

    [[nodiscard]] bool operator==(const Session<Data>& session) {
        return this == session.token();
    }
};

}
