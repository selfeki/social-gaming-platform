#pragma once

#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"
#include "arepa/game_spec/Timer.h"
#include <stack>

namespace gameSpecification::rule {

std::vector<std::string_view>
parseDotNotation(const std::string_view str);

std::vector<std::string>
evaluatelistExp(const Expression list);

ExpList 
getEvaluatedList(std::vector<std::string>listExpression, GameState state);


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

    void visitImpl(rule::Reverse&);

    void visitImpl(rule::Shuffle&);

    void visitImpl(rule::Discard&);

    void visitImpl(rule::Deal&);
    void visitImpl(rule::Timer&);

    void
    setGameState(GameState& gs) { state = gs; }

    GameState&
    getGameState() { return state; }

    Expression
    evaluateExpression(const Expression&);

    std::optional<Expression*>
    getExpressionPtr(const Expression&);


private:
    std::optional<Expression*>
    lookupLocalScope(const std::vector<std::string>&);

    std::optional<Expression*>
    lookupGlobalScope(const std::vector<std::string>&);

    std::optional<Expression*>
    lookupWithIndices(const std::vector<std::string>&);

    std::optional<Expression*>
    lookupName(std::string_view);

    std::string
    interpolateString(const std::string_view);

    InputRequest
    createInputRequest(InputType,
                       const Expression&,
                       const Expression&,
                       const Expression&,
                       const Expression&,
                       const std::optional<Expression>&);

    GameState& state;

};


}    // namespace gameSpecification::rule