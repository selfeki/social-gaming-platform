#include <vector>
#include <string>
#include "DSLInterpreter.h"


using IDType = std::string;


class GameInstance {
public:
  GameInstance(State& state)
    // use singleton  to generate unique ids?
    : IDType{id},
      Interpreter{},
      environments{extractEnvironments(state)},
      rules{extractRules(state)}
      { }

  // interprets rules until requires user interaction
  // updates passed in state based upon iterpretation of rule
  State
  updateState(State& state);

  // Possible duplication?
  // Have the environment be part of passed in state?
  Environments
  extractEnvironments(const State& state);

  // Also possible duplication?
  // Have access to rules from within state instead of extracting?
  std::vector<Node>
  extractRules(const State& state);

  IDType
  getGameID() { return gameID; }

private:
  IDType gameID;
  DSLInterpreter interpreter;
  std::vector<Node> rules;
  Environment environment;
  
};