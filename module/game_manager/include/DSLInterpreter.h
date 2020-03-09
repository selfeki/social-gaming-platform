#pragma once

#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

#include <stack>
#include <functional>

namespace gameSpecification::rule {


class InterpretVisitor : public RuleVisitor {
public:
  InterpretVisitor(GameState& state)
    : state{state}
      { }
  
  void visitImpl(const rule::ForEach& forEach);

  void
  setGameState(const GameState&);

private:
  GameState& state;

  std::stack<rule::Rule> context;
};

} // namespace gameSpecification::rule