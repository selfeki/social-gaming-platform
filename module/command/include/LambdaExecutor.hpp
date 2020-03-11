#pragma once

#include "Executor.hpp"

#include <functional>
#include <memory>

namespace arepa::command {


namespace {
    /**
     * An implementation of a (context-enabled) Executor created from a lambda.
     */
    template <typename User, typename Context>
    class LambdaExecutorWithContext : public Executor<User, Context> {
    private:
        using lambda_type = typename Executor<User, Context>::lambda_type;
        lambda_type _function;

    public:
        virtual ~LambdaExecutorWithContext() = default;

        LambdaExecutorWithContext(lambda_type function)
            : _function(function) {}

        void execute(Context context, User user, const CommandArguments& arguments) override {
            this->_function(context, user, arguments);
        };
    };


    /**
     * An implementation of a (no-context) Executor created from a lambda.
     */
    template <typename User>
    class LambdaExecutorWithoutContext : public Executor<User> {
    private:
        using lambda_type = typename Executor<User>::lambda_type;
        lambda_type _function;

    public:
        virtual ~LambdaExecutorWithoutContext() = default;

        LambdaExecutorWithoutContext(lambda_type function)
            : _function(function) {}

        void execute(User user, const CommandArguments& arguments) override {
            this->_function(user, arguments);
        }
    };

}

template <typename User, typename Context = void>
[[nodiscard]] std::unique_ptr<Executor<User, Context>> lambda_executor(typename Executor<User, Context>::lambda_type lambda) {
    using LambdaExecutor = std::conditional_t<std::is_void_v<Context>,
        LambdaExecutorWithoutContext<User>,
        LambdaExecutorWithContext<User, Context>>;

    return std::make_unique<LambdaExecutor>(lambda);
}

}