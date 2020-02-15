#include "DSLInterpreter.h"
#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rules.h"


class GameInstance {
public:
  GameInstance(GameState& gameState, const rules::RuleList& rules);

  // Interprets rules until requires user interaction or game ends.
  // It updates passed in state based upon interpretation of the current rule.
  //
  // todo: include fields in state specifying game conclusion or user input request
  // todo: include artificial rule at end of rule list representing game conclusion?
  void
  updateState();

  GameState&
  getGameState();


private:
  GameState&      gameState;
  DSLInterpreter  interpreter;
  rules::Rule     currentRule;
};
