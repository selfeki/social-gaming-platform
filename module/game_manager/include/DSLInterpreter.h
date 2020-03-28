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

    void visitImpl(rule::Reverse&);

    void visitImpl(rule::Shuffle&);

    void visitImpl(rule::Discard&);

    void visitImpl(rule::Deal&);

    void
    setGameState(GameState& gs) { state = gs; }

    GameState
    getGameState() { return state; }

private:
    GameState& state;
};


}    // namespace gameSpecification::rule