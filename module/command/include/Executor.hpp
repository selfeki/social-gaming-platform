#pragma once

#include "CommandArguments.hpp"
#include <functional>
#include <memory>

namespace arepa::command {

namespace {
    template <typename User, typename Context>
    class ExecutorWithContext {
    public:
        virtual ~ExecutorWithContext() = default;

        /**
         * Executes the command.
         *
         * @param context The context in which this command takes place.
         * @param user The user who initiated this command.
         * @param arguments The command arguments.
         */
        virtual void execute(Context& context, User& user, const CommandArguments& arguments) = 0;
    };

    template <typename User>
    class ExecutorWithoutContext {
    public:
        virtual ~ExecutorWithoutContext() = default;

        /**
         * Executes the command.
         *
         * @param user The user who initiated this command.
         * @param arguments The command arguments.
         */
        virtual void execute(User& user, const CommandArguments& arguments) = 0;
    };
}

/**
 * A command executor.
 * This is an interface that performs the function of a single command.
 */
template <typename User, typename Context = void>
class Executor : public std::conditional_t<std::is_void_v<Context>, ExecutorWithoutContext<User>, ExecutorWithContext<User, Context>> {
};


namespace {
    /**
     * An implementation of a (context-enabled) Executor created from a lambda.
     */
    template <typename User, typename Context>
    class LambdaExecutorWithContext : public Executor<User, Context> {
    public:
        using type = std::function<void(Context& context, User& user, const CommandArguments& arguments)>;

    private:
        type _function;

    public:
        virtual ~LambdaExecutorWithContext() = default;

        LambdaExecutorWithContext(type function)
            : _function(function) {}

        void execute(Context& context, User& user, const CommandArguments& arguments) override {
            this->_function(context, user, arguments);
        };
    };


    /**
     * An implementation of a (no-context) Executor created from a lambda.
     */
    template <typename User>
    class LambdaExecutorWithoutContext : public Executor<User> {
    public:
        using type = std::function<void(User& user, const CommandArguments& arguments)>;

    private:
        type _function;

    public:
        virtual ~LambdaExecutorWithoutContext() = default;

        LambdaExecutorWithoutContext(type function)
            : _function(function) {}

        void execute(User& user, const CommandArguments& arguments) override {
            this->_function(user, arguments);
        }
    };


}

template <typename User>
[[nodiscard]] std::unique_ptr<Executor<User>> lambda_executor(std::function<void(User&, const CommandArguments&)> lambda) {
    return std::make_unique<LambdaExecutorWithoutContext<User>>(lambda);
}

template <typename User, typename Context>
[[nodiscard]] std::unique_ptr<Executor<User, Context>> lambda_executor(std::function<void(Context&, User&, const CommandArguments&)> lambda) {
    return std::make_unique<LambdaExecutorWithContext<User, Context>>(lambda);
}

}