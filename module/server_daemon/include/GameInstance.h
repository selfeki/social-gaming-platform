#include <vector>
#include <string>
#include <DSLInterpreter.h>


using IDType = std::string;


class GameInstance {
public:
  GameInstance(State& state)
    // use singleton  to generate unique ids?
    : IDType{id},
      //This will create a dangling reference. 
      // It's a bit weird, but this will initialize the reference 
      // with the temporary DSLInterpreter constructed on this line.
      // todo: research why this is wrong.
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
  // Be careful about having the fields be references. 
  // Ask yourself what the ownership relationships are. 
  // If the object should own something, make sure it is not a reference.
  // If it should not own something, then a reference to that thing 
  // must be passed in as an argument to the constructor.
  DSLInterpreter interpreter;
  std::vector<Node> rules;
  Environment environment;
  
};