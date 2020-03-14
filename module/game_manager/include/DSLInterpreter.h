#pragma once

#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

#include <stack>

namespace gameSpecification::rule {


class InterpretVisitor : public RuleVisitor {
public:
    InterpretVisitor(GameState& state)
        : state { state }
        , needUserInput { false } {}

    void visitImpl(const rule::ForEach&);

    void visitImpl(const rule::GlobalMessage&);

    void visitImpl(const rule::InputChoice&);

    void
    setGameState(const GameState&);

private:
    GameState& state;

    bool needUserInput;

    std::stack<const Rule*> ruleStack;
};


}    // namespace gameSpecification::rule