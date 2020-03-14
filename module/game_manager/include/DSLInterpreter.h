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

    void visitImpl(rule::ForEach&);

    void visitImpl( rule::GlobalMessage&);

    void visitImpl( rule::Add&);

    void
    setGameState(const GameState&);

    std::stack<Rule*> scope;

private:
    GameState& state;

    bool needUserInput;

    ExpMap context;

};

}    // namespace gameSpecification::rule