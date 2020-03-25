#pragma once

#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

#include <stack>

namespace gameSpecification::rule {

std::vector<std::string_view>
parseDotNotation(const std::string_view str);


class InterpretVisitor : public RuleVisitor {
public:
    InterpretVisitor(GameState& gs)
        : state{ gs } 
        { }

    void visitImpl(rule::ForEach&);

    void visitImpl(rule::GlobalMessage&);

    void visitImpl(rule::Add&);

    void visitImpl(rule::InputText&);

    void visitImpl(rule::InputChoice&);

    void
    setGameState(GameState& gs) { state = gs; }

    GameState&
    getGameState() { return state; }

    // made public for testing purposes
    std::vector<std::string>
    tokenizeDotNotation(std::string_view);

    Expression
    evaluateExpression(const Expression&);
    
private:

    std::optional<Expression>
    lookupLocalScope(const std::vector<std::string>&);

    std::optional<Expression>
    lookupGlobalScope(const std::vector<std::string>&);

    std::optional<Expression>
    lookupWithIndices(const std::vector<std::string>&);

    std::optional<Expression>
    lookupWithString(std::string_view);

    std::string
    interpolateString(const std::string_view);

    GameState& state;
};


}    // namespace gameSpecification::rule