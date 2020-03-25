#pragma once

#include "arepa/game_spec/Expression.h"

using namespace gameSpecification;

class ExpressionPtr {
public:
    ExpressionPtr(const std::vector<std::string>&);

    std::optional<Expression*>
    getPtr(ExpMap&);

    void
    store(Expression, ExpMap&);

private:
    std::vector<std::string_view> indices;
};
