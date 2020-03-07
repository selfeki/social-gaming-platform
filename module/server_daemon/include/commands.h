#pragma once

#include <arepa/command/CommandName.hpp>
#include <arepa/command/Executor.hpp>
#include <arepa/server/Server.h>

#include <GameManager.h>
#include <deque>
#include <unordered_map>

/**
 * This is a really simple shim to help with refactoring the command module.
 */
struct CommandUser {
private:
    ::networking::ConnectionId _connection_id;
    std::deque<networking::Message> _messages_to_send;

public:
    CommandUser(::networking::ConnectionId& connection_id)
        : _connection_id(connection_id) {}

    ::networking::ConnectionId operator*() const {
        return this->_connection_id;
    }

    std::deque<networking::Message>& outgoing_message_queue() {
        return this->_messages_to_send;
    }
};

using CommandContext = GameManager;
using CommandExecutor = arepa::command::Executor<CommandUser, CommandContext>;

extern std::unordered_map<std::string, std::unique_ptr<CommandExecutor>> COMMAND_MAP;

void init_commands();