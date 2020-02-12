#include "DSLInterpreter.h"

#include <string>
#include <vector>

// TODO use boost::Visitor to interpret Abstract Syntax Tree of Rules.
// AST created with boost::variant.


// interprets current rule
// returns new state after interpretation
GameState
DSLInterpreter::interpret(Environment& env, GameState& state) {
    // TODO
}