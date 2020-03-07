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
    std::deque<messageReturn<::networking::ConnectionId>> _messages_to_send;

public:
    CommandUser(::networking::ConnectionId& connection_id)
        : _connection_id(connection_id) {}

    ::networking::ConnectionId operator*() const {
        return this->_connection_id;
    }

    std::deque<messageReturn<::networking::ConnectionId>>& outgoing_message_queue() {
        return this->_messages_to_send;
    }

    void send(const std::string& message) {
        this->_messages_to_send.push_back(messageReturn(this->_connection_id, message));
    }
};

using CommandContext = GameManager<::networking::ConnectionId>;
using CommandExecutor = arepa::command::Executor<CommandUser, CommandContext>;
using User = CommandUser;


extern std::unordered_map<std::string, std::unique_ptr<CommandExecutor>> COMMAND_MAP;

void init_commands();
void invalidCommand(CommandUser& user);
void unknownCommand(CommandUser& user);