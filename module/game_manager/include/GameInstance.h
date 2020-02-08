#include <vector>
#include <string>
#include "DSLInterpreter.h"

struct Rules; // todo: implement this


class GameInstance {
public:
  GameInstance(GameState& initGameState, Rules& rules);

  // interprets rules until requires user interaction
  // updates passed in state based upon iterpretation of rule
  GameState
  updateState(GameState& gameState);

  // todo: move this method to different component
  std::vector<Node>
  parseRules(const Rules& rules);

private:
// TODO
  // GameState gameState;
  // DSLInterpreter interpreter;
};