#pragma once

#include "CommandArguments.hpp"
#include "CommandName.hpp"
#include "Executor.hpp"

#include <arepa/Util.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace arepa::command {

/**
 * A class that holds a map of commands.
 */
template <typename User, typename Context = void>
class CommandMap {
public:
#pragma mark - Types -
    using Executor = arepa::command::Executor<User, Context>;


#pragma mark - Fields -
private:
    std::unordered_map<CommandName, std::shared_ptr<Executor>> _map;


#pragma mark - Constructors -
public:
    /**
     * Constructs a new command map.
     */
    CommandMap();


#pragma mark - Methods -
public:
    /**
     * Gets a command executor for a command name.
     * @param command_name The command name.
     * @return The corresponding command executor, or nullopt if none was set.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Executor>> find(const CommandName& command_name) const {
        return arepa::find_in_map<std::shared_ptr<Executor>>(this->_map, command_name);
    }

    /**
     * Sets a command executor.
     * 
     * @param command_name The command.
     * @param executor The lambda command executor.
     */
    template <typename _Lambda = std::conditional_t<std::is_void_v<Context>,
                  std::function<void(User&, const CommandArguments&)>,
                  std::function<void(Context&, User&, const CommandArguments&)>>>
    void insert(const CommandName& command_name, _Lambda executor) {
        this->insert(command_name, lambda_executor(executor));
    }

    /**
     * Sets a command executor.
     *
     * @param command_name The command.
     * @param executor The lambda command executor.
     */
    template <typename _Lambda = std::conditional_t<std::is_void_v<Context>,
                  std::function<void(User&, const CommandArguments&)>,
                  std::function<void(Context&, User&, const CommandArguments&)>>>
    void insert(const char* command_name, _Lambda executor) {
        this->insert(CommandName(std::string_view(command_name)), arepa::command::lambda_executor(executor));
    }

    /**
     * Sets a command executor.
     *
     * @param command_name The command.
     * @param executor The command executor.
     */
    void insert(const CommandName& command_name, std::shared_ptr<Executor> executor) {
        this->_map.emplace(command_name, executor);
    }

    /**
     * Sets a command executor.
     *
     * @param command_name The command.
     * @param executor The command executor.
     */
    void insert(const char* command_name, std::shared_ptr<Executor> executor) {
        this->insert.emplace(CommandName(std::string_view(command_name)), executor);
    }
};

}

#pragma mark - std::hash -
template <>
struct ::std::hash<arepa::command::CommandName> {
    std::size_t operator()(arepa::command::CommandName const& name) const noexcept {
        std::hash<std::string> hash;
        return hash(name);
    }
};
