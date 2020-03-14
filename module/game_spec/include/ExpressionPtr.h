#pragma once

#include "arepa/game_spec/Expression.h"

using namespace gameSpecification;

class ExpressionPtr {
public:
    ExpressionPtr(std::vector<std::string_view>);

    std::optional<Expression*>
    getPtr(ExpMap&);

private:
    std::vector<std::string_view> indices;
};
