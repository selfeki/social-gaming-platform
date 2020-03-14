#pragma once

#include "CommandArguments.hpp"

#include <functional>

namespace arepa::command {

namespace {
    template <typename User, typename Context>
    class ExecutorWithContext {
    public:
        using lambda_type = std::function<void(Context, User, const CommandArguments&)>;

        virtual ~ExecutorWithContext() = default;

        /**
         * Executes the command.
         *
         * @param context The context in which this command takes place.
         * @param user The user who initiated this command.
         * @param arguments The command arguments.
         */
        virtual void execute(Context context, User user, const CommandArguments& arguments) = 0;
    };

    template <typename User>
    class ExecutorWithoutContext {
    public:
        using lambda_type = std::function<void(User, const CommandArguments&)>;

        virtual ~ExecutorWithoutContext() = default;

        /**
         * Executes the command.
         *
         * @param user The user who initiated this command.
         * @param arguments The command arguments.
         */
        virtual void execute(User user, const CommandArguments& arguments) = 0;
    };
}

/**
 * A command executor.
 * This is an interface that performs the function of a single command.
 */
template <typename User, typename Context = void>
class Executor
    : public std::conditional_t<std::is_void_v<Context>, ExecutorWithoutContext<User>, ExecutorWithContext<User, Context>> {
};

}